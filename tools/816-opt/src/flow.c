/*---------------------------------------------------------------------------------

	Flow analysis for the 816-opt optimizer.

	Each code section (one tcc function) is parsed into instructions, its control
	flow graph is built (named and anonymous labels, branches, jumps) and the
	liveness of A, X, Y, the C/Z/N/V flags and the tcc pseudo registers
	(tcc__r0 .. tcc__f3h, 2 bytes each) is computed. Instructions whose results
	are all dead and which have no other effect are removed.

	The analysis is conservative wherever it does not know:
	- a call uses every register and kills none,
	- every register is live when leaving the function (rtl, unknown jump),
	- an 8-bit write does not kill a register (its high byte survives),
	- instructions inside .if blocks are never removed nor seen as kills,
	- memory other than the pseudo registers is never considered dead.

***************************************************************************/

#include "flow.h"

/* OPT816_DEBUG=1: trace the decisions on stderr */
static int dbg = -1;
#define DBG(...)                                        \
    do {                                                \
        if (dbg < 0)                                    \
            dbg = getenv("OPT816_DEBUG") != NULL;       \
        if (dbg) {                                      \
            fprintf(stderr, __VA_ARGS__);               \
            fputc('\n', stderr);                        \
        }                                               \
    } while (0)

typedef unsigned long long regset;

enum { R_A, R_X, R_Y, R_C, R_Z, R_N, R_V, R_PSEUDO = 8 };

static const char *slotNames[] = {"r0", "r0h", "r1", "r1h", "r2", "r2h", "r3", "r3h",
                                  "r4", "r4h", "r5", "r5h", "r9", "r9h", "r10", "r10h",
                                  "f0", "f0h", "f1", "f1h", "f2", "f2h", "f3", "f3h"};
#define NSLOT ((int) (sizeof(slotNames) / sizeof(slotNames[0])))
#define BIT(r) (1ULL << (r))
#define FLAGS_NZ (BIT(R_N) | BIT(R_Z))
#define ALLREGS ((1ULL << (R_PSEUDO + NSLOT)) - 1)
#define PSEUDOREGS (ALLREGS & ~((1ULL << R_PSEUDO) - 1))

enum { M_UNKNOWN = 0, M_16, M_8 };

typedef struct {
    char *text;        // line as in the input
    int isLabel;       // named label "name:"
    char anon[8];      // anonymous label defined by this line ("+", "--"...)
    int isDir;         // directive
    int isInsn;        // has an instruction
    char mn[4];        // mnemonic
    char size;         // 'b', 'w', 'l' or 0
    char op[256];      // operand, comment removed
    int keep;          // "DON'T OPTIMIZE" or inside .if
    int inIf;
    int succ[2], nsucc;
    int exits;         // leaves the function or unknown target
    regset exitLive;   // registers live when leaving
    int m;             // accumulator width before the instruction
    regset use, kill, defs;
    int side;          // effect other than registers: never removed
    regset in, out;
    int deleted;
    char *insBefore;   // instruction to insert before this line
    int busy;          // inside a rewrite of this round (X reserved)
    int dropLabel;     // removed with its anonymous label
} Line;

static int findSlot(const char *name, size_t len);
static regset slotBits(const char *names);
static long immValue(const char *op);
/* return value: r0 (+ r0h for pointers), r1/r1h for 32-bit values, f0 for floats */
#define RETREGS slotBits("r0 r0h r1 r1h f0 f0h")

static int findSlot(const char *name, size_t len)
{
    for (int k = 0; k < NSLOT; k++)
        if (strlen(slotNames[k]) == len && strncmp(slotNames[k], name, len) == 0)
            return k;
    return -1;
}

/* Pseudo register slots touched by an operand ("tcc__r0", "[tcc__r1],y",
   "(tcc__r2)", "tcc__r10 + 2"). Returns -1 if tcc__ is used in an unknown way,
   0 if the operand does not reference a pseudo register. *indirect is set for
   [..] (24-bit pointer: slot and the next one are read) */
static int operandSlots(const char *op, regset *slots, int *indirect)
{
    const char *p = strstr(op, "tcc__");
    *slots = 0;
    *indirect = 0;
    if (!p)
        return 0;
    if (strstr(p + 5, "tcc__"))
        return -1;
    const char *n = p + 5, *e = n;
    while (isalnum((unsigned char) *e))
        e++;
    int slot = findSlot(n, e - n);
    if (slot < 0)
        return -1;
    int offset = 0;
    while (*e == ' ')
        e++;
    if (*e == '+') {
        offset = atoi(e + 1);
        e++;
        while (*e == ' ' || isdigit((unsigned char) *e))
            e++;
    }
    int s = slot + offset / 2;
    if (s >= NSLOT)
        return -1;
    *slots = BIT(R_PSEUDO + s);
    if ((offset & 1) && s + 1 < NSLOT)
        *slots |= BIT(R_PSEUDO + s + 1);
    if (op[0] == '[') {
        *indirect = 1;
        if (s + 1 < NSLOT)
            *slots |= BIT(R_PSEUDO + s + 1);
    }
    // the operand must be exactly the register, possibly indexed
    if (op[0] != '[' && op[0] != '(' && p != op)
        return -1;
    return 1;
}

static int isAnonName(const char *s)
{
    if (!*s)
        return 0;
    for (const char *p = s; *p; p++)
        if (*p != s[0] || (s[0] != '+' && s[0] != '-'))
            return 0;
    return 1;
}

static void parseLine(Line *l, char *text)
{
    char buf[MAXLEN_LINE];
    memset(l, 0, sizeof(*l));
    l->text = text;
    strncpy(buf, text, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;

    char *c = strchr(buf, ';');
    if (c) {
        if (strstr(c, "DON'T OPTIMIZE"))
            l->keep = 1;
        *c = 0;
    }
    char *s = trimWhiteSpace(buf);
    if (!*s)
        return;
    if (s[0] == '.') {
        l->isDir = 1;
        return;
    }
    size_t len = strlen(s);
    if (s[len - 1] == ':' && !strchr(s, ' ')) {
        l->isLabel = 1;
        return;
    }
    if (s[0] == '+' || s[0] == '-') {
        size_t k = 0;
        while (s[k] == s[0])
            k++;
        if (k < sizeof(l->anon)) {
            memcpy(l->anon, s, k);
            l->anon[k] = 0;
        }
        s += k;
        while (*s == ' ')
            s++;
        if (!*s)
            return;
    }
    l->isInsn = 1;
    for (int k = 0; k < 3 && s[k]; k++)
        l->mn[k] = tolower((unsigned char) s[k]);
    l->mn[3] = 0;
    char *p = s + 3;
    if (p[0] == '.' && p[1]) {
        l->size = p[1];
        p += 2;
    }
    while (*p == ' ')
        p++;
    strncpy(l->op, p, sizeof(l->op) - 1);
    char *end = l->op + strlen(l->op);
    while (end > l->op && end[-1] == ' ')
        *--end = 0;
}

static int is(const Line *l, const char *mn)
{
    return strcmp(l->mn, mn) == 0;
}

static int isBranch(const Line *l)
{
    static const char *b[] = {"bcc", "bcs", "beq", "bne", "bmi", "bpl", "bvc", "bvs", "bra", "brl"};
    for (size_t k = 0; k < sizeof(b) / sizeof(b[0]); k++)
        if (is(l, b[k]))
            return 1;
    return 0;
}

static regset branchUse(const Line *l)
{
    if (is(l, "bcc") || is(l, "bcs"))
        return BIT(R_C);
    if (is(l, "beq") || is(l, "bne"))
        return BIT(R_Z);
    if (is(l, "bmi") || is(l, "bpl"))
        return BIT(R_N);
    if (is(l, "bvc") || is(l, "bvs"))
        return BIT(R_V);
    return 0;
}

/* Registers effects of one instruction, given the accumulator width m.
   Returns 0 if the instruction is not understood (barrier). */
static int effects(Line *l, int m)
{
    regset slots = 0;
    int indirect = 0, ref = 0;
    const char *op = l->op;
    int accOp = (!*op || strcmp(op, "a") == 0);
    int imm = (op[0] == '#');

    if (!accOp && !imm) {
        ref = operandSlots(op, &slots, &indirect);
        if (ref < 0)
            return 0;
    }
    regset idx = 0;
    size_t ol = strlen(op);
    if (ol > 2 && op[ol - 2] == ',' && (op[ol - 1] == 'x' || op[ol - 1] == 'X'))
        idx = BIT(R_X);
    if (ol > 2 && op[ol - 2] == ',' && (op[ol - 1] == 'y' || op[ol - 1] == 'Y'))
        idx = BIT(R_Y);
    if (strstr(op, ",s"))
        idx = 0;

    /* operand read / write helpers */
    regset rd = idx | (ref ? slots : 0);       // reading the operand
    regset wrUse = idx | (indirect ? slots : 0); // registers read when writing it
    int wrPseudo = (ref && !indirect && op[0] != '(');
    int full = (m == M_16);

    regset aDef = BIT(R_A);
    regset aKill = full ? BIT(R_A) : 0;
    regset aPart = full ? 0 : BIT(R_A); // 8-bit or unknown width: high byte kept

#define STORE(src, isfull)                                         \
    do {                                                          \
        l->use |= (src) | wrUse;                                  \
        if (wrPseudo) {                                           \
            l->defs |= slots;                                     \
            if (isfull)                                           \
                l->kill |= slots;                                 \
            else                                                  \
                l->use |= slots;                                  \
        } else                                                    \
            l->side = 1;                                          \
    } while (0)

    if (is(l, "lda")) {
        l->use |= rd | aPart;
        l->defs |= aDef | FLAGS_NZ;
        l->kill |= aKill | FLAGS_NZ;
    } else if (is(l, "ldx") || is(l, "ldy")) {
        regset r = BIT(is(l, "ldx") ? R_X : R_Y);
        l->use |= rd;
        l->defs |= r | FLAGS_NZ;
        l->kill |= r | FLAGS_NZ;
    } else if (is(l, "sta")) {
        STORE(BIT(R_A), full);
    } else if (is(l, "stx") || is(l, "sty")) {
        STORE(BIT(is(l, "stx") ? R_X : R_Y), 1);
    } else if (is(l, "stz")) {
        STORE(0, full);
    } else if (is(l, "adc") || is(l, "sbc")) {
        l->use |= rd | BIT(R_A) | BIT(R_C);
        l->defs |= aDef | FLAGS_NZ | BIT(R_C) | BIT(R_V);
        l->kill |= aKill | FLAGS_NZ | BIT(R_C) | BIT(R_V);
    } else if (is(l, "and") || is(l, "ora") || is(l, "eor")) {
        l->use |= rd | BIT(R_A);
        l->defs |= aDef | FLAGS_NZ;
        l->kill |= aKill | FLAGS_NZ;
    } else if (is(l, "cmp") || is(l, "cpx") || is(l, "cpy")) {
        regset r = BIT(is(l, "cmp") ? R_A : is(l, "cpx") ? R_X : R_Y);
        l->use |= rd | r;
        l->defs |= FLAGS_NZ | BIT(R_C);
        l->kill |= FLAGS_NZ | BIT(R_C);
    } else if (is(l, "bit")) {
        regset f = imm ? BIT(R_Z) : (FLAGS_NZ | BIT(R_V));
        l->use |= rd | BIT(R_A);
        l->defs |= f;
        l->kill |= f;
    } else if (is(l, "inc") || is(l, "dec") || is(l, "ina") || is(l, "dea")) {
        if (accOp || is(l, "ina") || is(l, "dea")) {
            l->use |= BIT(R_A);
            l->defs |= BIT(R_A) | FLAGS_NZ;
            l->kill |= FLAGS_NZ;
        } else {
            STORE(rd, 0);
            l->defs |= FLAGS_NZ;
            l->kill |= FLAGS_NZ;
        }
    } else if (is(l, "asl") || is(l, "lsr") || is(l, "rol") || is(l, "ror")) {
        regset c = (is(l, "rol") || is(l, "ror")) ? BIT(R_C) : 0;
        if (accOp) {
            l->use |= BIT(R_A) | c;
            l->defs |= BIT(R_A) | FLAGS_NZ | BIT(R_C);
            l->kill |= FLAGS_NZ | BIT(R_C);
        } else {
            STORE(rd | c, 0);
            l->defs |= FLAGS_NZ | BIT(R_C);
            l->kill |= FLAGS_NZ | BIT(R_C);
        }
    } else if (is(l, "inx") || is(l, "dex") || is(l, "iny") || is(l, "dey")) {
        regset r = BIT(l->mn[2] == 'x' ? R_X : R_Y);
        l->use |= r;
        l->defs |= r | FLAGS_NZ;
        l->kill |= FLAGS_NZ;
    } else if (is(l, "tax") || is(l, "tay") || is(l, "txy") || is(l, "tyx")) {
        regset s = BIT(l->mn[1] == 'a' ? R_A : l->mn[1] == 'x' ? R_X : R_Y);
        regset d = BIT(l->mn[2] == 'x' ? R_X : R_Y);
        l->use |= s;
        l->defs |= d | FLAGS_NZ;
        l->kill |= d | FLAGS_NZ;
    } else if (is(l, "txa") || is(l, "tya")) {
        l->use |= BIT(l->mn[1] == 'x' ? R_X : R_Y) | aPart;
        l->defs |= aDef | FLAGS_NZ;
        l->kill |= aKill | FLAGS_NZ;
    } else if (is(l, "tsa") || is(l, "tsc") || is(l, "tdc") || is(l, "tda")) {
        l->defs |= aDef | FLAGS_NZ;
        l->kill |= BIT(R_A) | FLAGS_NZ;
    } else if (is(l, "tsx")) {
        l->defs |= BIT(R_X) | FLAGS_NZ;
        l->kill |= BIT(R_X) | FLAGS_NZ;
    } else if (is(l, "xba")) {
        l->use |= BIT(R_A);
        l->defs |= BIT(R_A) | FLAGS_NZ;
        l->kill |= BIT(R_A) | FLAGS_NZ;
    } else if (is(l, "clc") || is(l, "sec")) {
        l->defs |= BIT(R_C);
        l->kill |= BIT(R_C);
    } else if (is(l, "clv")) {
        l->defs |= BIT(R_V);
        l->kill |= BIT(R_V);
    } else if (is(l, "nop")) {
        l->side = 1; // may be a delay (hardware registers)
    } else if (is(l, "pha") || is(l, "phx") || is(l, "phy")) {
        l->use |= BIT(l->mn[2] == 'a' ? R_A : l->mn[2] == 'x' ? R_X : R_Y);
        l->side = 1;
    } else if (is(l, "pei")) {
        l->use |= slots;
        l->side = 1;
    } else if (is(l, "pea") || is(l, "per") || is(l, "phb") || is(l, "phd") || is(l, "phk")) {
        l->side = 1;
    } else if (is(l, "php")) {
        l->use |= BIT(R_C) | FLAGS_NZ | BIT(R_V);
        l->side = 1;
    } else if (is(l, "pla") || is(l, "plx") || is(l, "ply")) {
        regset r = BIT(l->mn[2] == 'a' ? R_A : l->mn[2] == 'x' ? R_X : R_Y);
        if (r == BIT(R_A) && !full)
            l->use |= BIT(R_A);
        l->defs |= r | FLAGS_NZ;
        l->kill |= (r == BIT(R_A) ? aKill : r) | FLAGS_NZ;
        l->side = 1;
    } else if (is(l, "plb") || is(l, "pld")) {
        l->defs |= FLAGS_NZ;
        l->kill |= FLAGS_NZ;
        l->side = 1;
    } else if (is(l, "tcs") || is(l, "tas") || is(l, "tcd") || is(l, "tad")) {
        l->use |= BIT(R_A);
        l->side = 1;
    } else if (is(l, "txs")) {
        l->use |= BIT(R_X);
        l->side = 1;
    } else if (is(l, "sep") || is(l, "rep")) {
        int mask = (int) strtol(op + (op[0] == '#' ? 1 : 0) + (op[1] == '$' ? 1 : 0), NULL, 16);
        if (mask & 0x01)
            l->kill |= BIT(R_C), l->defs |= BIT(R_C);
        if (mask & 0x02)
            l->kill |= BIT(R_Z), l->defs |= BIT(R_Z);
        if (mask & 0x40)
            l->kill |= BIT(R_V), l->defs |= BIT(R_V);
        if (mask & 0x80)
            l->kill |= BIT(R_N), l->defs |= BIT(R_N);
        l->side = 1;
    } else if (isBranch(l)) {
        l->use |= branchUse(l);
        l->side = 1;
    } else
        return 0;
#undef STORE
    return 1;
}

static regset slotBits(const char *names)
{
    char buf[64];
    regset r = 0;
    strncpy(buf, names, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = 0;
    for (char *t = strtok(buf, " "); t; t = strtok(NULL, " ")) {
        int k = findSlot(t, strlen(t));
        if (k >= 0)
            r |= BIT(R_PSEUDO + k);
    }
    return r;
}

/* Registers read by a called routine. C functions take their arguments on the
   stack; the tcc__ helpers of libtcc.asm have their own inputs; unknown
   helpers (floats...) read everything. */
static regset callUse(const char *name)
{
    static const struct {
        const char *name, *slots;
        regset hw;
    } helpers[] = {
        {"tcc__mul", "r9 r10", 0},
        {"tcc__mull", "r9 r9h r10 r10h", 0},
        {"tcc__udiv", "", BIT(R_A) | BIT(R_X)}, // x = dividend, a = divisor
        {"tcc__div", "", BIT(R_A) | BIT(R_X)},
        {"tcc__jsl_r10", "r10 r10h", 0},      // call through a function pointer
        {"tcc__jsl_ind_r9", "r9 r9h", 0},
        {"tcc__divdi3", "", 0},               // arguments on the stack
        {"tcc__moddi3", "", 0},
        {"tcc__udivdi3", "", 0},
        {"tcc__umoddi3", "", 0},
        {"tcc__shldi3", "", 0},
        {"tcc__sardi3", "", 0},
        {"tcc__shrdi3", "", 0},
    };
    if (!startWith(name, "tcc__"))
        return 0;
    for (size_t k = 0; k < sizeof(helpers) / sizeof(helpers[0]); k++)
        if (strcmp(name, helpers[k].name) == 0)
            return slotBits(helpers[k].slots) | helpers[k].hw;
    return ALLREGS;
}

static int findLabel(Line *f, int n, const char *name)
{
    size_t len = strlen(name);
    for (int k = 0; k < n; k++)
        if (f[k].isLabel && strncmp(f[k].text, name, len) == 0 && f[k].text[len] == ':')
            return k;
    return -1;
}

static int findAnon(Line *f, int n, int from, const char *name)
{
    if (name[0] == '+') {
        for (int k = from + 1; k < n; k++)
            if (strcmp(f[k].anon, name) == 0)
                return k;
    } else {
        for (int k = from - 1; k >= 0; k--)
            if (strcmp(f[k].anon, name) == 0)
                return k;
    }
    return -1;
}

/* registers live at the return of the function being analysed */
static regset curRetLive;

/* Returns 0 if the function cannot be analysed */
static int analyse(Line *f, int n)
{
    int depth = 0;

    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        l->nsucc = 0;
        l->exits = 0;
        l->exitLive = ALLREGS;
        l->busy = 0;
        l->use = l->kill = l->defs = 0;
        l->side = 0;
        if (l->isDir) {
            const char *t = l->text;
            if (startWith(t, ".if") || startWith(t, ".IF"))
                depth++;
            else if ((startWith(t, ".endif") || startWith(t, ".ENDIF")) && depth > 0)
                depth--;
            else if (!startWith(t, ".else") && !startWith(t, ".ELSE") && !startWith(t, ".define ")) {
                DBG("directive inconnue: %s", t);
                return 0; // data or unknown directive inside the code
            }
        }
        l->inIf = depth > 0;
        if (l->isInsn && (is(l, "sep") || is(l, "rep"))) {
            const char *o = l->op + 1 + (l->op[1] == '$');
            if (strtol(o, NULL, 16) & 0x10)
                return 0; // 8-bit index registers: not handled
        }
    }

    /* control flow */
    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        int next = (i + 1 < n) ? i + 1 : -1;
        if (!l->isInsn) {
            if (next >= 0)
                l->succ[l->nsucc++] = next;
            else
                l->exits = 1;
            continue;
        }
        if (isBranch(l) || is(l, "jmp") || is(l, "jml")) {
            int t = -1;
            if (!is(l, "jml") && l->op[0] != '(' && l->op[0] != '[') {
                t = isAnonName(l->op) ? findAnon(f, n, i, l->op) : findLabel(f, n, l->op);
            }
            if (t < 0)
                l->exits = 1;
            else
                l->succ[l->nsucc++] = t;
            if (!is(l, "bra") && !is(l, "brl") && !is(l, "jmp") && !is(l, "jml") && next >= 0)
                l->succ[l->nsucc++] = next;
        } else if (is(l, "rtl") || is(l, "rts")) {
            /* tcc returns its value in pseudo registers; the epilogue itself
               overwrites A, and callers never read A, X, Y or the flags */
            l->exits = 1;
            l->exitLive = curRetLive;
        } else if (is(l, "rti") || is(l, "stp")) {
            l->exits = 1;
        } else if (next >= 0) {
            l->succ[l->nsucc++] = next;
        } else
            l->exits = 1;
    }

    /* accumulator width, forward */
    for (int i = 0; i < n; i++)
        f[i].m = -1;
    f[0].m = M_16;
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < n; i++) {
            Line *l = &f[i];
            if (l->m < 0)
                continue;
            int m = l->m;
            if (l->isInsn && (is(l, "sep") || is(l, "rep"))) {
                const char *o = l->op + 1 + (l->op[1] == '$');
                if (strtol(o, NULL, 16) & 0x20)
                    m = is(l, "sep") ? M_8 : M_16;
            } else if (l->isInsn && (is(l, "plp") || is(l, "rti") || is(l, "xce")))
                m = M_UNKNOWN;
            for (int k = 0; k < l->nsucc; k++) {
                Line *s = &f[l->succ[k]];
                int nm = s->m < 0 ? m : (s->m == m ? m : M_UNKNOWN);
                if (nm != s->m) {
                    s->m = nm;
                    changed = 1;
                }
            }
        }
    }

    /* register effects */
    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        if (!l->isInsn)
            continue;
        int m = l->m < 0 ? M_UNKNOWN : l->m; // unreachable: be conservative
        if (is(l, "jsr") || is(l, "jsl")) {
            /* C calls pass their arguments on the stack; the tcc__ helpers
               may take them in A, X, Y */
            /* only long calls to a plain name follow the C convention */
            int plain = (is(l, "jsl") || l->size == 'l') && (isalpha((unsigned char) l->op[0]) || l->op[0] == '_')
                        && !strpbrk(l->op, " ,()[]") && !startWith(l->op, "move_");
            l->use = plain ? callUse(l->op) : ALLREGS;
            l->side = 1;
        } else if (is(l, "rtl") || is(l, "rts")) {
            l->use = curRetLive;
            l->side = 1;
        } else if (is(l, "jmp") || is(l, "jml") || is(l, "rti")) {
            if (l->exits)
                l->use = ALLREGS; // unknown target (a local jmp is a plain edge)
            l->side = 1;
        } else if (!effects(l, m)) {
            l->use = ALLREGS;
            l->side = 1;
        }
        if (l->inIf) {
            l->kill = 0;
            l->side = 1;
        }
        if (l->keep)
            l->side = 1;
    }

    /* liveness, backward */
    for (int i = 0; i < n; i++)
        f[i].in = f[i].out = 0;
    changed = 1;
    while (changed) {
        changed = 0;
        for (int i = n - 1; i >= 0; i--) {
            Line *l = &f[i];
            regset out = l->exits ? l->exitLive : 0;
            for (int k = 0; k < l->nsucc; k++)
                out |= f[l->succ[k]].in;
            regset in = l->use | (out & ~l->kill);
            if (in != l->in || out != l->out) {
                l->in = in;
                l->out = out;
                changed = 1;
            }
        }
    }
    return 1;
}

/* Dead code: instructions without side effect whose results are all dead */
static int removeDead(Line *f, int n)
{
    int removed = 0;
    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        if (!l->isInsn || l->side || l->deleted || l->m < 0)
            continue;
        if ((l->defs & l->out) == 0 && l->defs) {
            l->deleted = 1;
            removed++;
        }
    }
    return removed;
}

/* "#NAME" or "#NAME + 0" -> NAME (identifier), else NULL */
static int immSymbol(const char *op, char *name, size_t size)
{
    if (op[0] != '#' || op[1] == ':')
        return 0;
    const char *p = op + 1, *e = p;
    while (isalnum((unsigned char) *e) || *e == '_' || *e == '{' || *e == '}')
        e++;
    if (e == p || isdigit((unsigned char) *p) || (*e && strcmp(e, " + 0") != 0))
        return 0;
    if ((size_t) (e - p) >= size)
        return 0;
    memcpy(name, p, e - p);
    name[e - p] = 0;
    return 1;
}

static int isBlockBoundary(const Line *l)
{
    return !l->isInsn || l->anon[0] || isBranch(l) || is(l, "jmp") || is(l, "jml") || is(l, "jsr")
           || is(l, "jsl") || is(l, "rtl") || is(l, "rts") || is(l, "rti") || l->inIf;
}

static void setText(Line *l, const char *text)
{
    char *t = strdup(text);
    if (!t)
        fatal("malloc-lines");
    char *ins = l->insBefore;
    parseLine(l, t);
    l->insBefore = ins;
}

static int changesStack(const Line *l)
{
    static const char *m[] = {"pha", "phx", "phy", "php", "phb", "phd", "phk", "pea", "pei", "per",
                              "pla", "plx", "ply", "plp", "plb", "pld", "tas", "tcs", "txs", "jsr",
                              "jsl", "rtl", "rts"};
    for (size_t k = 0; k < sizeof(m) / sizeof(m[0]); k++)
        if (is(l, m[k]))
            return 1;
    return 0;
}

/* Does X hold the value of the operand `src` (a stack slot "...,s" or a
   pseudo register) just before line `at`? Looks back in the block for
   "lda src / tax" or "ldx src" with src unchanged since. */
static int xHolds(Line *f, int at, const char *src)
{
    int stackSlot = endWith(src, ",s");
    regset slots = 0;
    int ind;
    if (!stackSlot && (operandSlots(src, &slots, &ind) != 1 || ind))
        return 0;
    for (int j = at - 1; j >= 1 && !isBlockBoundary(&f[j]); j--) {
        Line *l = &f[j];
        if (l->deleted || l->insBefore)
            return 0;
        if (l->defs & BIT(R_X)) {
            if (is(l, "ldx") && strcmp(l->op, src) == 0)
                return 1;
            Line *p = &f[j - 1];
            return is(l, "tax") && !p->deleted && p->isInsn && !p->anon[0] && is(p, "lda")
                   && strcmp(p->op, src) == 0 && p->m == M_16;
        }
        /* the value of src must not change */
        if (stackSlot) {
            if (changesStack(l) || (strchr(l->op, '[') && !is(l, "lda"))
                || ((is(l, "sta") || is(l, "stz") || is(l, "stx") || is(l, "sty") || is(l, "inc")
                     || is(l, "dec") || is(l, "asl") || is(l, "lsr") || is(l, "rol") || is(l, "ror")
                     || is(l, "tsb") || is(l, "trb"))
                    && endWith(l->op, ",s")))
                return 0;
        } else if (l->defs & slots)
            return 0;
    }
    return 0;
}

/* Global array access through a pointer register:
        lda.w #:SYM / sta.b tcc__rKh ... lda.w #SYM + 0 / clc / adc.b tcc__rP
        [/ field offset] / sta.b tcc__rK ... lda.b [tcc__rK] / sta.b [tcc__rK]
   The index is copied to X before the addition and the accesses become
   lda.l SYM + C,x / sta.l SYM + C,x; the address computation is then dead.
   The field offset C of an array of structures (tab[i].field) is added after
   the index, as "inc a" (C <= 4) or "clc / adc.w #C" (C < $8000). */

/* Field offset lines after the index addition at line i: returns the line of
   the store into the pointer register and sets *field (0 when none). */
static int fieldAfterIndex(Line *f, int n, int i, long *field)
{
    int k = i + 1;
    *field = 0;
    while (k < n && f[k].isInsn && strcmp(f[k].text, "inc a") == 0 && !f[k].anon[0] && !f[k].busy
           && !f[k].insBefore && !f[k].deleted && f[k].m == M_16 && *field < 4) {
        (*field)++;
        k++;
    }
    if (!*field && k + 1 < n && f[k].isInsn && is(&f[k], "clc") && !f[k].anon[0] && !f[k].busy
        && !f[k].insBefore && f[k + 1].isInsn && is(&f[k + 1], "adc") && !f[k + 1].anon[0] && !f[k + 1].busy
        && !f[k + 1].insBefore && f[k + 1].m == M_16) {
        long c = immValue(f[k + 1].op);
        if (c > 0 && c < 0x8000) {
            *field = c;
            k += 2;
        }
    }
    return k;
}

static int indexArrays(Line *f, int n)
{
    int done = 0;
    char sym[256], buf[300];

    for (int i = 2; i + 1 < n; i++) {
        long field;
        int s = fieldAfterIndex(f, n, i, &field);
        if (s >= n)
            continue;
        Line *adc = &f[i], *clc = &f[i - 1], *st = &f[s];
        if (!adc->isInsn || !is(adc, "adc") || adc->size != 'b' || adc->m != M_16 || adc->anon[0]
            || !clc->isInsn || !is(clc, "clc") || clc->anon[0] || clc->insBefore || clc->busy
            || adc->busy || st->busy || !st->isInsn
            || !is(st, "sta") || st->size != 'b' || st->anon[0] || st->m != M_16)
            { if (adc->isInsn && is(adc, "adc")) DBG("idx L%d rejet 0: %s m=%d st=[%s] m=%d clc=[%s]", i, adc->text, adc->m, st->text, st->m, clc->text); continue; }
        regset pSlot, kSlot;
        int ind;
        if (operandSlots(adc->op, &pSlot, &ind) != 1 || ind || adc->op[0] != 't'
            || operandSlots(st->op, &kSlot, &ind) != 1 || ind || st->op[0] != 't'
            || strchr(adc->op, '+') || strchr(st->op, '+'))
            { DBG("idx L%d rejet %d: %s", i, 626, adc->text); continue; }
        const char *kName = st->op + 5; // "rN"
        if (kName[strlen(kName) - 1] == 'h')
            { DBG("idx L%d rejet %d: %s", i, 629, adc->text); continue; }

        /* which operand is the address: A (lda before clc) or rP */
        int useTax = 0;
        Line *prev = &f[i - 2];
        if (prev->isInsn && !prev->anon[0] && is(prev, "lda") && immSymbol(prev->op, sym, sizeof(sym))) {
            useTax = 0; // A = SYM, index in rP
        } else {
            /* rP = SYM, stored in this block, index in A */
            int j = i - 2, found = 0;
            for (; j >= 1 && !isBlockBoundary(&f[j]); j--) {
                if (f[j].defs & pSlot) {
                    found = is(&f[j], "sta") && f[j].size == 'b' && strcmp(f[j].op, adc->op) == 0
                            && f[j - 1].isInsn && !f[j - 1].anon[0] && is(&f[j - 1], "lda")
                            && immSymbol(f[j - 1].op, sym, sizeof(sym)) && f[j].m == M_16;
                    break;
                }
            }
            if (!found)
                { DBG("idx L%d rejet 648: %s | %s | %s | %s", i, adc->text, f[i-2].text, f[i-3].text, i > 3 ? f[i-4].text : ""); continue; }
            useTax = 1;
        }
        /* bank register rKh: last write before the addition, in this block */
        char bankOp[64];
        snprintf(bankOp, sizeof(bankOp), "tcc__%sh", kName);
        regset hSlot;
        if (operandSlots(bankOp, &hSlot, &ind) != 1)
            { DBG("idx L%d rejet %d: %s", i, 659, adc->text); continue; }
        int bankOk = 0;
        for (int j = i - 1; j >= 1 && !isBlockBoundary(&f[j]); j--) {
            if (f[j].defs & hSlot) {
                snprintf(buf, sizeof(buf), "#:%s", sym);
                bankOk = is(&f[j], "sta") && f[j].size == 'b' && strcmp(f[j].op, bankOp) == 0
                         && f[j].m == M_16 && f[j - 1].isInsn && !f[j - 1].anon[0]
                         && is(&f[j - 1], "lda") && strcmp(f[j - 1].op, buf) == 0;
                break;
            }
        }
        if (!bankOk)
            { DBG("idx L%d rejet %d: %s", i, 671, adc->text); continue; }

        /* uses of rK / rKh after the addition: only [rK] accesses */
        regset ptr = kSlot | hSlot;
        char derefOp[64];
        snprintf(derefOp, sizeof(derefOp), "[tcc__%s]", kName);
        int deref[32], nderef = 0, ok = 0, xChanged = 0, xFree = 1, xLive = 0, j;
        for (j = s + 1; j < n; j++) {
            Line *l = &f[j];
            if (!(l->in & ptr)) {
                ok = 1; // pointer dead from here
                break;
            }
            if (isBlockBoundary(l) || l->insBefore || l->busy)
                break;
            if (l->use & ptr) {
                if (!((is(l, "lda") || is(l, "sta")) && l->size == 'b' && strcmp(l->op, derefOp) == 0)
                    || nderef == 32)
                    break;
                if (xChanged)
                    xFree = 0; // X does not keep the index up to this access
                if (l->in & BIT(R_X))
                    xLive = 1; // X live at the access: no mode possible
                deref[nderef++] = j;
            }
            if (l->defs & ptr)
                break; // pointer changed while still live
            if (l->defs & BIT(R_X))
                xChanged = 1;
        }
        /* reuse mode: X already holds the index (a[i] = b[i] ...) */
        const char *idxSrc = useTax ? (is(prev, "lda") && !prev->busy ? prev->op : "") : adc->op;
        int reuse = *idxSrc && xFree && xHolds(f, i - 1, idxSrc);
        if (useTax && is(prev, "txa") && !prev->busy && !prev->anon[0] && xFree)
            reuse = 1; // the index in A was just copied from X

        if (ok != 1 || !nderef || (xLive && !reuse))
            { DBG("idx L%d rejet 698: %s stop L%d [%s] ok=%d nderef=%d x=%d in=%llx", i, adc->text, j, j < n ? f[j].text : "", ok, nderef, xChanged, j < n ? f[j].in : 0ULL); continue; }

        if (reuse) {
            /* nothing to insert: X keeps the index up to the last access */
            for (int k = i - 1; k < j; k++)
                f[k].busy = 1;
        } else if (xFree && !(clc->in & BIT(R_X))) {
            /* direct mode: the index stays in X from the addition to the
               last access; the address computation becomes dead */
            clc->insBefore = strdup(useTax ? "tax" : (snprintf(buf, sizeof(buf), "ldx.b %s", adc->op), buf));
            for (int k = i - 1; k < j; k++)
                f[k].busy = 1;
        } else {
            /* index mode: rK receives the index instead of the address, and
               each access loads it in X. A and the flags must be dead after
               the store, since their values change. */
            regset changed = BIT(R_A) | FLAGS_NZ | BIT(R_C) | BIT(R_V);
            if ((st->out & changed) || prev->busy || prev->insBefore)
                { DBG("idx L%d rejet 760: %s", i, adc->text); continue; }
            if (!useTax) {
                snprintf(buf, sizeof(buf), "lda.b %s", adc->op);
                setText(prev, buf); // A = index instead of SYM
            }
            clc->deleted = 1;
            adc->deleted = 1;
            for (int k = i + 1; k < s; k++)
                f[k].deleted = 1; // the field offset goes into the access
            for (int k = 0; k < nderef; k++) {
                snprintf(buf, sizeof(buf), "ldx.b tcc__%s", kName);
                f[deref[k]].insBefore = strdup(buf);
            }
            for (int k = i - 2; k < j; k++)
                f[k].busy = 1;
        }
        for (int k = 0; k < nderef; k++) {
            Line *l = &f[deref[k]];
            if (field)
                snprintf(buf, sizeof(buf), "%s.l %s + %ld,x", l->mn, sym, field);
            else
                snprintf(buf, sizeof(buf), "%s.l %s,x", l->mn, sym);
            DBG("idx L%d %s: %s -> %s (L%d)", i, reuse ? "reuse" : clc->insBefore && !clc->deleted ? "direct" : "index", l->text, buf, deref[k]);
            setText(l, buf);
        }
        done++;
    }
    return done;
}

/* Structure fields: tcc adds the field offset to the pointer, then
   dereferences it:
        clc / lda.b tcc__rK / adc.w #C / sta.b tcc__rK ... op.b [tcc__rK]
   The pointer is left unchanged and each access becomes
        ldy.w #C / op.b [tcc__rK],y
   rK must only be used by these accesses (then dead), Y free at each one, and
   A and the flags dead after the addition. C < $8000: [dp],y carries into the
   bank, a "negative" offset would not wrap like the 16-bit addition. */
static int isDerefOp(const Line *l)
{
    static const char *m[] = {"lda", "sta", "adc", "sbc", "and", "ora", "eor", "cmp"};
    for (size_t k = 0; k < sizeof(m) / sizeof(m[0]); k++)
        if (is(l, m[k]))
            return 1;
    return 0;
}

/* Is the pseudo register rI, just before line `at`, a value in 0..$7fff? Its
   last write in the block must be a zero-extended byte
   (lda.w #0 / sep #$20 / lda ... / rep #$20 / sta rI) or an "and" with a
   mask below $8000 just before the store. */
static int smallIndex(Line *f, int at, const char *op, regset slot)
{
    for (int j = at - 1; j >= 1 && !isBlockBoundary(&f[j]); j--) {
        Line *l = &f[j];
        if (l->deleted || l->insBefore)
            return 0;
        if (!(l->defs & slot))
            continue;
        if (!is(l, "sta") || l->size != 'b' || strcmp(l->op, op) != 0 || l->m != M_16)
            return 0;
        Line *p = &f[j - 1];
        if (p->isInsn && is(p, "and") && p->op[0] == '#' && !p->anon[0]) {
            char *e;
            const char *num = p->op + 1;
            long mask = num[0] == '$' ? strtol(num + 1, &e, 16) : strtol(num, &e, 10);
            return *e == 0 && mask >= 0 && mask < 0x8000;
        }
        if (j < 4)
            return 0;
        Line *rep = &f[j - 1], *ld = &f[j - 2], *sep = &f[j - 3], *zero = &f[j - 4];
        return rep->isInsn && strcmp(rep->text, "rep #$20") == 0 && ld->isInsn && is(ld, "lda") && ld->m == M_8
               && sep->isInsn && strcmp(sep->text, "sep #$20") == 0 && zero->isInsn && is(zero, "lda")
               && (strcmp(zero->op, "#0") == 0) && !rep->anon[0] && !ld->anon[0] && !sep->anon[0];
    }
    return 0;
}

static int fieldOffsets(Line *f, int n)
{
    int done = 0;
    char buf[128];

    for (int i = 0; i + 4 < n; i++) {
        Line *clc = &f[i], *ld = &f[i + 1], *adc = &f[i + 2], *st = &f[i + 3];
        if (!clc->isInsn || !is(clc, "clc") || !ld->isInsn || !is(ld, "lda") || ld->size != 'b'
            || !adc->isInsn || !is(adc, "adc") || !st->isInsn || !is(st, "sta")
            || st->size != 'b' || strcmp(ld->op, st->op) != 0 || ld->m != M_16 || st->m != M_16)
            continue;
        int bad = 0;
        for (int j = i; j <= i + 3; j++)
            if (f[j].anon[0] || f[j].busy || f[j].insBefore || f[j].deleted)
                bad = 1;
        if (bad)
            continue;
        /* offset: a constant 1..$7fff, or a pseudo register proven 0..$7fff */
        long c = -1;
        regset iSlot = 0;
        int ind;
        if (adc->op[0] == '#') {
            const char *num = adc->op + 1;
            char *endp;
            c = num[0] == '$' ? strtol(num + 1, &endp, 16) : strtol(num, &endp, 10);
            if (*endp || c <= 0 || c >= 0x8000)
                continue;
        } else if (adc->size == 'b' && adc->op[0] == 't' && !strchr(adc->op, '+')
                   && operandSlots(adc->op, &iSlot, &ind) == 1 && !ind) {
            if (!smallIndex(f, i + 2, adc->op, iSlot))
                continue;
        } else
            continue;
        regset kSlot;
        if (operandSlots(st->op, &kSlot, &ind) != 1 || ind || st->op[0] != 't' || strchr(st->op, '+')
            || endWith(st->op, "h"))
            continue;
        if (st->out & (BIT(R_A) | FLAGS_NZ | BIT(R_C) | BIT(R_V)))
            continue;
        char hOp[64], derefOp[64];
        regset hSlot;
        snprintf(hOp, sizeof(hOp), "%sh", st->op);
        snprintf(derefOp, sizeof(derefOp), "[%s]", st->op);
        if (operandSlots(hOp, &hSlot, &ind) != 1)
            continue;
        regset ptr = kSlot | hSlot;
        if (iSlot & ptr)
            continue;

        /* two ways to have the offset in Y at each access:
           - one ldy where the addition was, Y then untouched up to the last
             access (Y must be free there);
           - one ldy before each access (Y free at each one; for a register
             index, the register must still hold it) */
        int deref[32], nderef = 0, ok = 0, j, idxChanged = 0, yChanged = 0;
        int oneLdy = !(clc->in & BIT(R_Y)), eachLdy = 1;
        for (j = i + 4; j < n; j++) {
            Line *l = &f[j];
            if (!(l->in & ptr)) {
                ok = 1;
                break;
            }
            if (isBlockBoundary(l) || l->insBefore || l->busy)
                break;
            if (l->use & ptr) {
                if (!isDerefOp(l) || l->size != 'b' || strcmp(l->op, derefOp) != 0 || nderef == 32)
                    break;
                if (yChanged)
                    oneLdy = 0;
                if ((l->in & BIT(R_Y)) || idxChanged)
                    eachLdy = 0;
                deref[nderef++] = j;
            }
            if (l->defs & ptr)
                break;
            if (l->defs & iSlot)
                idxChanged = 1; // the index register no longer holds the index
            if (l->defs & BIT(R_Y))
                yChanged = 1;
        }
        if (!ok || !nderef || (!oneLdy && !eachLdy))
            continue;

        char ldy[80];
        if (c >= 0)
            snprintf(ldy, sizeof(ldy), "ldy.w #%ld", c);
        else
            snprintf(ldy, sizeof(ldy), "ldy.b %s", adc->op);
        for (int k = i; k <= i + 3; k++)
            f[k].deleted = 1;
        for (int k = i; k < j; k++)
            f[k].busy = 1;
        if (oneLdy)
            clc->insBefore = strdup(ldy); // emitted in place of the removed addition
        for (int k = 0; k < nderef; k++) {
            Line *l = &f[deref[k]];
            if (!oneLdy)
                l->insBefore = strdup(ldy);
            snprintf(buf, sizeof(buf), "%s.b %s,y", l->mn, derefOp);
            setText(l, buf);
        }
        DBG("champ %s via %s (%d acces)", adc->op, st->op, nderef);
        done++;
    }
    return done;
}

/* ---------------------------------------------------------------------------
   Available values: which locations hold the same value.
   Locations: A, X, Y, the pseudo registers, the stack slots ("...,s") and the
   immediate constants ("#..."). A state gives each location a class number;
   two locations with the same class hold the same 16-bit value. At merges, two
   locations stay equal only if they are equal on every path.
   --------------------------------------------------------------------------- */
#define VL_A 0
#define VL_X 1
#define VL_Y 2
#define VL_PSEUDO 3
#define VL_MAX 256

typedef struct {
    int nloc;
    char *text[VL_MAX]; // stack slot or constant operand (NULL for A, X, Y, pseudo)
    int isStack[VL_MAX];
    int stackOff[VL_MAX]; // leading number of a stack slot, for overlaps
    int offOk[VL_MAX];
    int stackEscapes;     // the address of a stack slot may be taken
} VLocs;

static int vlFind(VLocs *v, const char *op, int add)
{
    for (int k = VL_PSEUDO + NSLOT; k < v->nloc; k++)
        if (strcmp(v->text[k], op) == 0)
            return k;
    if (!add || v->nloc >= VL_MAX)
        return -1;
    int k = v->nloc++;
    v->text[k] = strdup(op);
    v->isStack[k] = endWith(op, ",s");
    char *e;
    v->stackOff[k] = (int) strtol(op, &e, 10);
    v->offOk[k] = v->isStack[k] && e != op;
    return k;
}

/* location of an operand, -1 if not tracked */
static int vlOperand(VLocs *v, const char *op, int add)
{
    if (op[0] == '#')
        return vlFind(v, op, add);
    if (endWith(op, ",s") && !strchr(op, '(') && !strchr(op, '['))
        return vlFind(v, op, add);
    regset s;
    int ind;
    if (op[0] == 't' && !strchr(op, ',') && !strchr(op, '+') && operandSlots(op, &s, &ind) == 1 && !ind)
        for (int k = 0; k < NSLOT; k++)
            if (s == BIT(R_PSEUDO + k))
                return VL_PSEUDO + k;
    return -1;
}

static void vlFresh(int *st, int nloc, int loc)
{
    int m = 0;
    for (int k = 0; k < nloc; k++)
        if (st[k] > m)
            m = st[k];
    st[loc] = m + 1;
}

static void vlFreshStack(VLocs *v, int *st, int around, int all)
{
    for (int k = VL_PSEUDO + NSLOT; k < v->nloc; k++) {
        if (!v->isStack[k] || k == around)
            continue;
        const char *a = around >= 0 ? strchr(v->text[around], ' ') : NULL;
        const char *b = strchr(v->text[k], ' ');
        int overlap = all || !v->offOk[k] || around < 0 || !v->offOk[around] || !a || !b || strcmp(a, b) != 0
                      || abs(v->stackOff[k] - v->stackOff[around]) < 2;
        if (overlap)
            vlFresh(st, v->nloc, k);
    }
}

static void vlCanon(int *st, int nloc)
{
    int map[VL_MAX * 2 + 2], from[VL_MAX * 2 + 2], nm = 0;
    for (int k = 0; k < nloc; k++) {
        int c = st[k], j;
        for (j = 0; j < nm && from[j] != c; j++)
            ;
        if (j == nm) {
            from[nm] = c;
            map[nm] = nm;
            nm++;
        }
        st[k] = map[j];
    }
}

static void vlMeet(int *dst, const int *a, const int *b, int nloc)
{
    int pa[VL_MAX], pb[VL_MAX], n = 0;
    for (int k = 0; k < nloc; k++) {
        int j;
        for (j = 0; j < n && !(pa[j] == a[k] && pb[j] == b[k]); j++)
            ;
        if (j == n) {
            pa[n] = a[k];
            pb[n] = b[k];
            n++;
        }
        dst[k] = j;
    }
}

static void vlTransfer(VLocs *v, const Line *l, const int *in, int *out)
{
    int nl = v->nloc;
    memcpy(out, in, nl * sizeof(int));
    if (!l->isInsn)
        return;
    int m16 = (l->m == M_16);
    int opLoc = vlOperand(v, l->op, 0);
    int isStackOp = opLoc >= 0 && v->isStack[opLoc];

    if (l->inIf || is(l, "jsr") || is(l, "jsl") || l->use == ALLREGS) {
        for (int k = 0; k < nl; k++)
            vlFresh(out, nl, k); // unknown effect: nothing is known any more
        for (int k = 0; k < nl; k++)
            if (v->text[k] && v->text[k][0] == '#')
                out[k] = -1 - k; // constants keep distinct fixed classes
        vlCanon(out, nl);
        return;
    }
    if (changesStack(l))
        vlFreshStack(v, out, -1, 1);

    if (is(l, "lda") || is(l, "ldx") || is(l, "ldy")) {
        int r = is(l, "lda") ? VL_A : is(l, "ldx") ? VL_X : VL_Y;
        if (opLoc >= 0 && (r != VL_A || m16))
            out[r] = out[opLoc];
        else
            vlFresh(out, nl, r);
    } else if (is(l, "sta") || is(l, "stx") || is(l, "sty") || is(l, "stz")) {
        int full = (is(l, "sta") || is(l, "stz")) ? m16 : 1; // 8-bit: one byte written
        int src = is(l, "sta") ? VL_A : is(l, "stx") ? VL_X : is(l, "sty") ? VL_Y : vlFind(v, "#0", 0);
        int pseudo = opLoc >= VL_PSEUDO && opLoc < VL_PSEUDO + NSLOT;
        if (pseudo || isStackOp) {
            if (isStackOp)
                vlFreshStack(v, out, opLoc, 0); // overlapping slots
            if (full && src >= 0)
                out[opLoc] = out[src];
            else
                vlFresh(out, nl, opLoc);
        } else if (v->stackEscapes)
            vlFreshStack(v, out, -1, 1); // store through a pointer: may hit a local
    } else if (is(l, "tax") || is(l, "tay") || is(l, "txy") || is(l, "tyx")) {
        int s = l->mn[1] == 'a' ? VL_A : l->mn[1] == 'x' ? VL_X : VL_Y;
        int d = l->mn[2] == 'x' ? VL_X : VL_Y;
        out[d] = out[s];
    } else if (is(l, "txa") || is(l, "tya")) {
        if (m16)
            out[VL_A] = out[l->mn[1] == 'x' ? VL_X : VL_Y];
        else
            vlFresh(out, nl, VL_A);
    } else {
        /* any other write: fresh value */
        if (l->defs & BIT(R_A))
            vlFresh(out, nl, VL_A);
        if (l->defs & BIT(R_X))
            vlFresh(out, nl, VL_X);
        if (l->defs & BIT(R_Y))
            vlFresh(out, nl, VL_Y);
        for (int k = 0; k < NSLOT; k++)
            if (l->defs & BIT(R_PSEUDO + k))
                vlFresh(out, nl, VL_PSEUDO + k);
        if (isStackOp && (is(l, "inc") || is(l, "dec") || is(l, "asl") || is(l, "lsr") || is(l, "rol")
                          || is(l, "ror") || is(l, "tsb") || is(l, "trb"))) {
            vlFreshStack(v, out, opLoc, 0);
            vlFresh(out, nl, opLoc);
        } else if (l->side && v->stackEscapes && !isBranch(l))
            vlFreshStack(v, out, -1, 1);
    }
    vlCanon(out, nl);
}

/* read-modify-write instruction that also exists on A */
static int isRmwA(const Line *l)
{
    return l->isInsn
           && (is(l, "inc") || is(l, "dec") || is(l, "asl") || is(l, "lsr") || is(l, "rol") || is(l, "ror"));
}

static int redundantValues(Line *f, int n)
{
    VLocs v;
    memset(&v, 0, sizeof(v));
    v.nloc = VL_PSEUDO + NSLOT;
    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        if (!l->isInsn)
            continue;
        if (!l->inIf && (is(l, "tsa") || is(l, "tsc") || is(l, "tsx")))
            v.stackEscapes = 1;
        vlOperand(&v, l->op, 1);
    }
    vlFind(&v, "#0", 1);
    int nl = v.nloc, done = 0;

    int *in = malloc(sizeof(int) * nl * n), *out = malloc(sizeof(int) * nl * n);
    char *known = calloc(n, 1);
    if (!in || !out || !known)
        fatal("malloc-lines");
    /* predecessors */
    int *npred = calloc(n, sizeof(int)), **pred = calloc(n, sizeof(int *));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < f[i].nsucc; k++)
            npred[f[i].succ[k]]++;
    for (int i = 0; i < n; i++) {
        pred[i] = malloc(sizeof(int) * (npred[i] + 1));
        npred[i] = 0;
    }
    for (int i = 0; i < n; i++)
        for (int k = 0; k < f[i].nsucc; k++) {
            int s = f[i].succ[k];
            pred[s][npred[s]++] = i;
        }

    int *tmp = malloc(sizeof(int) * nl);
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < n; i++) {
            int have = 0;
            if (i == 0) {
                for (int k = 0; k < nl; k++)
                    tmp[k] = k; // entry: everything distinct
                have = 1;
            }
            for (int p = 0; p < npred[i]; p++) {
                int q = pred[i][p];
                if (!known[q])
                    continue;
                if (!have) {
                    memcpy(tmp, &out[q * nl], nl * sizeof(int));
                    have = 1;
                } else {
                    int m[VL_MAX];
                    vlMeet(m, tmp, &out[q * nl], nl);
                    memcpy(tmp, m, nl * sizeof(int));
                }
            }
            if (!have)
                continue;
            vlCanon(tmp, nl);
            if (known[i] && memcmp(tmp, &in[i * nl], nl * sizeof(int)) == 0)
                continue;
            memcpy(&in[i * nl], tmp, nl * sizeof(int));
            vlTransfer(&v, &f[i], &in[i * nl], &out[i * nl]);
            known[i] = 1;
            changed = 1;
        }
    }

    /* rewrites (each one keeps every value unchanged) */
    for (int i = 0; i < n; i++) {
        Line *l = &f[i];
        if (!known[i] || !l->isInsn || l->keep || l->inIf || l->busy || l->insBefore || l->deleted
            || l->anon[0] || l->m != M_16)
            continue;
        const int *st = &in[i * nl];
        int op = vlOperand(&v, l->op, 0);
        if (op < 0)
            continue;
        /* inc.b rX / asl.b rX ... / lda.b rX with A == rX before: the same
           operations on A, then sta.b rX (same values and flags: each
           operation sets N/Z/C as on memory, and the lda set N/Z from the
           final value like the last operation; the store goes away later if
           rX is dead) */
        if (isRmwA(l) && l->size == 'b' && op >= VL_PSEUDO && op < VL_PSEUDO + NSLOT && st[VL_A] == st[op]) {
            int j = i;
            while (j < n && isRmwA(&f[j]) && f[j].size == 'b' && strcmp(f[j].op, l->op) == 0 && !f[j].anon[0]
                   && !f[j].busy && !f[j].insBefore && !f[j].deleted && !f[j].keep && !f[j].inIf
                   && f[j].m == M_16)
                j++;
            Line *nx = j < n ? &f[j] : NULL;
            /* ending with ldx.b rX / ldy.b rX and A dead after: same on A, then
               sta.b rX / tax (tax sets N/Z from the same value as ldx) */
            int toX = nx && (is(nx, "ldx") || is(nx, "ldy")) && !(nx->out & BIT(R_A));
            if (nx && nx->isInsn && (is(nx, "lda") || toX) && nx->size == 'b' && strcmp(nx->op, l->op) == 0
                && !nx->anon[0] && !nx->busy && !nx->insBefore && !nx->deleted && !nx->keep && !nx->inIf
                && nx->m == M_16) { // X/Y are 16 bits in every analysed function
                char t[300], store[300];
                snprintf(store, sizeof(store), "sta.b %s", l->op); // before l is rewritten
                for (int k = i; k < j; k++) {
                    snprintf(t, sizeof(t), "%s a", f[k].mn);
                    DBG("valeur: %s -> %s", f[k].text, t);
                    setText(&f[k], t);
                }
                if (toX) {
                    nx->insBefore = strdup(store);
                    setText(nx, is(nx, "ldx") ? "tax" : "tay");
                } else
                    setText(nx, store);
                done++;
                i = j; // the following lines have just been rewritten
            }
            continue;
        }
        /* operation reading a pseudo register that holds a constant: read the
           constant (faster); that holds a stack slot, and dead after: read the
           slot (the copy into the register then becomes dead) */
        if ((is(l, "adc") || is(l, "sbc") || is(l, "cmp") || is(l, "and") || is(l, "ora") || is(l, "eor"))
            && l->size == 'b' && op >= VL_PSEUDO && op < VL_PSEUDO + NSLOT) {
            int imm = -1, slot = -1;
            for (int k = VL_PSEUDO + NSLOT; k < nl; k++)
                if (st[k] == st[op]) {
                    if (v.text[k][0] == '#' && imm < 0)
                        imm = k;
                    else if (v.isStack[k] && slot < 0)
                        slot = k;
                }
            char t[300];
            if (imm >= 0)
                snprintf(t, sizeof(t), "%s.w %s", l->mn, v.text[imm]);
            else if (slot >= 0 && !(l->out & BIT(R_PSEUDO + (op - VL_PSEUDO))))
                snprintf(t, sizeof(t), "%s %s", l->mn, v.text[slot]);
            else
                continue;
            DBG("valeur: %s -> %s", l->text, t);
            setText(l, t);
            done++;
            continue;
        }
        int nzDead = !(l->out & FLAGS_NZ);
        if (is(l, "lda") || is(l, "ldx") || is(l, "ldy")) {
            int r = is(l, "lda") ? VL_A : is(l, "ldx") ? VL_X : VL_Y;
            if (st[r] == st[op]) {
                if (nzDead) {
                    l->deleted = 1;
                    done++;
                    DBG("valeur: %s supprime", l->text);
                }
                continue;
            }
            const char *rep = NULL;
            if (r == VL_A)
                rep = st[VL_X] == st[op] ? "txa" : st[VL_Y] == st[op] ? "tya" : NULL;
            else if (st[VL_A] == st[op])
                rep = r == VL_X ? "tax" : "tay";
            else if (st[r == VL_X ? VL_Y : VL_X] == st[op])
                rep = r == VL_X ? "tyx" : "txy";
            if (rep) {
                DBG("valeur: %s -> %s", l->text, rep);
                setText(l, rep);
                done++;
            }
        } else if ((is(l, "sta") || is(l, "stx") || is(l, "sty") || is(l, "stz")) && op >= VL_PSEUDO) {
            int src = is(l, "sta") ? VL_A : is(l, "stx") ? VL_X : is(l, "sty") ? VL_Y : vlFind(&v, "#0", 0);
            if (src >= 0 && st[op] == st[src]) {
                l->deleted = 1;
                done++;
                DBG("valeur: %s supprime", l->text);
            }
        }
    }

    for (int i = 0; i < n; i++)
        free(pred[i]);
    free(pred);
    free(npred);
    free(in);
    free(out);
    free(known);
    free(tmp);
    for (int k = VL_PSEUDO + NSLOT; k < v.nloc; k++)
        free(v.text[k]);
    return done;
}

/* Byte operation on a byte variable ("u8 v |= expr"): tcc zero-extends both
   operands, operates in 16 bits and stores the low byte:
        lda.w #0 / sep #$20 / lda V / rep #$20 / sta.b rT
        lda.w #0 / sep #$20 / lda E / rep #$20 / ora.b rT / sep #$20 / sta V / rep #$20
   becomes, in 8 bits:
        sep #$20 / lda E / ora V / sta V / rep #$20
   (same for and, eor). rT must be dead after, A and N/Z too (their values
   change), V a stack slot or pseudo register, E must not use rT. */
/* Value of an immediate operand "#123" / "#$7B", -1 if not a plain number */
static long immValue(const char *op)
{
    char *e;
    long v;
    if (op[0] != '#')
        return -1;
    v = op[1] == '$' ? strtol(op + 2, &e, 16) : strtol(op + 1, &e, 10);
    return (*e == 0 && e != op + 1) ? v : -1;
}

/* A is known to be in [0, $7FFF] just before line i: the previous instruction
   (same block) is "and #N" with N < $8000, or the end of a zero-extended byte
   read (lda #0 / sep #$20 / lda X / rep #$20). */
static int aIsSmall(Line *f, int i)
{
    int k[4], nk = 0;
    for (int j = i - 1; j >= 0 && nk < 4; j--) {
        if (f[j].deleted)
            continue;
        if (!f[j].isInsn || f[j].anon[0] || f[j].inIf || f[j].insBefore)
            break;
        k[nk++] = j;
    }
    if (nk < 1)
        return 0;
    Line *p = &f[k[0]];
    if (is(p, "and") && p->m == M_16) {
        long mask = immValue(p->op);
        return mask >= 0 && mask < 0x8000;
    }
    return nk == 4 && strcmp(p->text, "rep #$20") == 0 && is(&f[k[1]], "lda") && f[k[1]].m == M_8
           && strcmp(f[k[2]].text, "sep #$20") == 0 && is(&f[k[3]], "lda") && strcmp(f[k[3]].op, "#0") == 0;
}

/* Signed comparison of a value known to be in [0, $7FFF] with a constant in
   [0, $7FFF] (a u8 promoted to int, "for (i = 0; i < 64; i++)"): signed and
   unsigned orders agree, so the overflow test of the signed form goes away:
        sec / sbc #c / [beq L /] bvc + / eor #$8000 / + / bmi T  (bpl T)
        =>  cmp #c / [beq L /] bcc T  (bcs T)
   C is the same after cmp as after sec / sbc, Z too (A == c); A, N, V and Z
   must be dead after the last branch (A keeps its value instead of A - c). */
static int unsignedCompares(Line *f, int n)
{
    int done = 0;
    char buf[300];

    for (int i = 1; i + 5 < n; i++) {
        Line *sec = &f[i], *sbc = &f[i + 1];
        if (!sec->isInsn || !is(sec, "sec") || sec->anon[0] || !sbc->isInsn || !is(sbc, "sbc") || sbc->anon[0]
            || sbc->m != M_16)
            continue;
        long c = immValue(sbc->op);
        if (c < 0 || c >= 0x8000)
            continue;
        int j = i + 2;
        if (f[j].isInsn && is(&f[j], "beq") && !f[j].anon[0])
            j++; // the equality test reads Z, the same after cmp
        if (j + 3 >= n)
            continue;
        Line *bvc = &f[j], *eor = &f[j + 1], *plus = &f[j + 2], *br = &f[j + 3];
        if (!bvc->isInsn || !is(bvc, "bvc") || strcmp(bvc->op, "+") != 0 || bvc->anon[0] || !eor->isInsn
            || !is(eor, "eor") || immValue(eor->op) != 0x8000 || eor->anon[0] || plus->isInsn
            || strcmp(plus->anon, "+") != 0 || !br->isInsn || br->anon[0] || !(is(br, "bmi") || is(br, "bpl")))
            continue;
        int bad = 0;
        for (int k = i; k <= j + 3; k++)
            if (f[k].deleted || f[k].busy || f[k].insBefore || f[k].keep || f[k].inIf)
                bad = 1;
        if (bad || (br->out & (BIT(R_A) | BIT(R_N) | BIT(R_V) | BIT(R_Z))) || !aIsSmall(f, i))
            continue;
        /* the "+" label goes away: no other branch may target it */
        for (int k = j - 1; k >= 0 && strcmp(f[k].anon, "+") != 0; k--)
            if (f[k].isInsn && strcmp(f[k].op, "+") == 0)
                bad = 1;
        if (bad)
            continue;
        snprintf(buf, sizeof(buf), "cmp.w #%ld", c);
        DBG("comparaison non signee: %s / %s -> %s", sbc->text, br->text, buf);
        sec->deleted = 1;
        setText(sbc, buf);
        bvc->deleted = eor->deleted = 1;
        plus->deleted = 1;
        plus->dropLabel = 1;
        snprintf(buf, sizeof(buf), "%s %s", is(br, "bmi") ? "bcc" : "bcs", br->op);
        setText(br, buf);
        done++;
        i = j + 3;
    }
    return done;
}

/* Increment or decrement of a byte variable ("u8 v; v++"): tcc zero-extends
   it into a pseudo register, increments the register and stores the low byte:
        lda.w #0 / sep #$20 / lda V / rep #$20 / sta.b rT / inc.b rT
        / sep #$20 / lda.b rT / sta V / rep #$20
   becomes, in 8 bits:
        sep #$20 / inc V / rep #$20                 (V absolute or direct page)
        sep #$20 / lda V / inc a / sta V / rep #$20 (V long or a stack slot)
   N and Z end the same (set by the 8-bit result in both cases), C is not
   touched; A and rT must be dead after (A keeps another value). */
static int byteIncrements(Line *f, int n)
{
    int done = 0;
    char buf[300];

    for (int i = 0; i + 9 < n; i++) {
        Line *z = &f[i];
        if (!(z->isInsn && is(z, "lda") && strcmp(z->op, "#0") == 0 && z->m == M_16))
            continue;
        int bad = 0;
        for (int j = i; j <= i + 9; j++)
            if (!f[j].isInsn || f[j].anon[0] || f[j].busy || f[j].insBefore || f[j].deleted || f[j].keep
                || f[j].inIf)
                bad = 1;
        if (bad)
            continue;
        Line *ld = &f[i + 2], *st = &f[i + 4], *op = &f[i + 5], *ld2 = &f[i + 7], *sv = &f[i + 8],
             *rep = &f[i + 9];
        if (strcmp(f[i + 1].text, "sep #$20") != 0 || !is(ld, "lda") || strcmp(f[i + 3].text, "rep #$20") != 0
            || !is(st, "sta") || st->size != 'b' || !(is(op, "inc") || is(op, "dec")) || op->size != 'b'
            || strcmp(op->op, st->op) != 0 || strcmp(f[i + 6].text, "sep #$20") != 0 || !is(ld2, "lda")
            || ld2->size != 'b' || strcmp(ld2->op, st->op) != 0 || !is(sv, "sta") || strcmp(rep->text, "rep #$20") != 0)
            continue;
        /* same variable, same addressing, not a pointer nor indexed */
        if (strcmp(ld->op, sv->op) != 0 || ld->size != sv->size || strchr(ld->op, '[') || strchr(ld->op, '(')
            || ld->op[0] == '#' || strstr(ld->op, "tcc__") || (strchr(ld->op, ',') && !endWith(ld->op, ",s")))
            continue;
        regset slots;
        int ind;
        if (operandSlots(st->op, &slots, &ind) != 1 || ind || (rep->out & (slots | BIT(R_A))))
            continue;
        const char *rmw = is(op, "inc") ? "inc" : "dec";
        int direct = !endWith(ld->op, ",s") && (ld->size == 'w' || ld->size == 'b');
        for (int j = i; j <= i + 9; j++)
            f[j].deleted = 1;
        if (direct)
            snprintf(buf, sizeof(buf), "sep #$20\n%s.%c %s\nrep #$20", rmw, ld->size, ld->op);
        else
            snprintf(buf, sizeof(buf), "sep #$20\n%s\n%s a\n%s\nrep #$20", ld->text, rmw, sv->text);
        f[i].insBefore = strdup(buf);
        DBG("octet %s: %s", rmw, ld->op);
        done++;
        i += 9;
    }
    return done;
}

static int byteOps(Line *f, int n)
{
    int done = 0;
    char buf[300];

    for (int i = 0; i + 12 < n; i++) {
        Line *l = &f[i];
        if (!(l->isInsn && is(l, "lda") && strcmp(l->op, "#0") == 0 && l->m == M_16))
            continue;
        int bad = 0;
        for (int j = i; j <= i + 12; j++)
            if (!f[j].isInsn || f[j].anon[0] || f[j].busy || f[j].insBefore || f[j].deleted || f[j].keep
                || f[j].inIf)
                bad = 1;
        if (bad)
            continue;
        Line *v1 = &f[i + 2], *st = &f[i + 4], *z2 = &f[i + 5], *e = &f[i + 7], *op = &f[i + 9],
             *sv = &f[i + 11];
        if (strcmp(f[i + 1].text, "sep #$20") != 0 || !is(v1, "lda") || strcmp(f[i + 3].text, "rep #$20") != 0
            || !is(st, "sta") || st->size != 'b' || !is(z2, "lda") || strcmp(z2->op, "#0") != 0
            || strcmp(f[i + 6].text, "sep #$20") != 0 || !is(e, "lda") || strcmp(f[i + 8].text, "rep #$20") != 0
            || !(is(op, "ora") || is(op, "and") || is(op, "eor")) || op->size != 'b'
            || strcmp(op->op, st->op) != 0 || strcmp(f[i + 10].text, "sep #$20") != 0 || !is(sv, "sta")
            || strcmp(sv->op, v1->op) != 0 || strcmp(f[i + 12].text, "rep #$20") != 0)
            continue;
        /* V: a stack slot or a pseudo register (no side effect, same text) */
        regset vs, ts, es;
        int ind;
        int vStack = endWith(v1->op, ",s") && !strchr(v1->op, '(') && !strchr(v1->op, '[');
        int vPseudo = operandSlots(v1->op, &vs, &ind) == 1 && !ind && v1->op[0] == 't' && !strchr(v1->op, ',');
        if (!vStack && !vPseudo)
            continue;
        if (operandSlots(st->op, &ts, &ind) != 1 || ind || st->op[0] != 't')
            continue;
        if (operandSlots(e->op, &es, &ind) < 0 || (es & ts) || (vPseudo && (vs & ts)))
            continue;
        if (op->out & ts)
            continue; // the temporary is still needed
        if (f[i + 12].out & (BIT(R_A) | FLAGS_NZ))
            continue; // A (high byte) and N/Z change
        const char *vop = v1->op;
        f[i].insBefore = NULL;
        snprintf(buf, sizeof(buf), "sep #$20\n%s%s %s\n%s%s %s\nsta%s %s\nrep #$20", e->mn,
                 e->size ? (e->size == 'b' ? ".b" : e->size == 'w' ? ".w" : ".l") : "", e->op, op->mn,
                 v1->size ? (v1->size == 'b' ? ".b" : v1->size == 'w' ? ".w" : ".l") : "", vop,
                 sv->size ? (sv->size == 'b' ? ".b" : sv->size == 'w' ? ".w" : ".l") : "", vop);
        f[i].insBefore = strdup(buf);
        for (int j = i; j <= i + 12; j++) {
            f[j].deleted = 1;
            f[j].busy = 1;
        }
        DBG("octet: %s %s |%s", op->mn, vop, e->op);
        done++;
    }
    return done;
}

/* Constant shift loops generated by tcc for 5..15 bits:
        ldy.w #N / - / asl a (or lsr a, or cmp #$8000 + ror a) / dey / bne -
   are unrolled (xba + and for 8 bits at once). The loop leaves Y = 0 and
   N/Z from dey: they must be dead after it. */
static int unrollShifts(Line *f, int n)
{
    int done = 0;
    char buf[512];

    for (int i = 0; i + 4 < n; i++) {
        Line *ldy = &f[i], *lab = &f[i + 1];
        if (!ldy->isInsn || !is(ldy, "ldy") || ldy->op[0] != '#' || ldy->anon[0] || ldy->busy
            || ldy->insBefore || lab->isInsn || strcmp(lab->anon, "-") != 0)
            continue;
        int count = atoi(ldy->op + 1);
        if (ldy->op[1] == '$' || count < 2 || count > 15)
            continue;
        int body = i + 2, len;
        const char *kind;
        if (f[body].isInsn && !f[body].anon[0] && (strcmp(f[body].text, "asl a") == 0 || strcmp(f[body].text, "lsr a") == 0)) {
            len = 1;
            kind = f[body].text;
        } else if (body + 1 < n && strcmp(f[body].text, "cmp #$8000") == 0 && strcmp(f[body + 1].text, "ror a") == 0) {
            len = 2;
            kind = "sar";
        } else
            continue;
        int dey = body + len, bne = dey + 1;
        if (bne + 1 >= n || strcmp(f[dey].text, "dey") != 0 || strcmp(f[bne].text, "bne -") != 0)
            continue;
        Line *after = &f[bne + 1];
        if (after->in & (BIT(R_Y) | FLAGS_NZ))
            continue;
        /* no other branch to this "-" */
        int other = 0;
        for (int j = bne + 1; j < n && strcmp(f[j].anon, "-") != 0; j++)
            if (f[j].isInsn && strcmp(f[j].op, "-") == 0)
                other = 1;
        for (int j = i; j <= bne; j++)
            if (f[j].busy || f[j].insBefore)
                other = 1;
        if (other || f[body].m != M_16)
            continue;

        buf[0] = 0;
        int k = count;
        if (count > 8 && strcmp(kind, "sar") != 0) {
            strcat(buf, kind[0] == 'a' ? "xba\nand.w #$ff00\n" : "xba\nand.w #$00ff\n");
            k -= 8;
        }
        while (k-- > 0)
            strcat(buf, strcmp(kind, "sar") == 0 ? "cmp #$8000\nror a\n" : (kind[0] == 'a' ? "asl a\n" : "lsr a\n"));
        buf[strlen(buf) - 1] = 0;
        ldy->insBefore = strdup(buf);
        for (int j = i; j <= bne; j++) {
            f[j].deleted = 1;
            f[j].busy = 1;
        }
        lab->dropLabel = 1;
        DBG("shift %s x%d unrolled", kind, count);
        done++;
    }
    return done;
}

static int isCodeSection(const char *line)
{
    return startWith(line, ".SECTION") && !startWith(line, ".SECTION \".data")
           && !strstr(line, "RAMSECTION");
}

typedef struct {
    size_t hdr, end;   // .SECTION and .ENDS lines in the file
    Line *f;
    int n, ok;
    char name[256];    // first label of the section
    int isStatic;      // tccs_ symbol: every call is in this file
    int addrTaken;     // name used otherwise than by a call
    regset retLive;    // return registers read by the callers
    regset retMax;     // return registers of the declared return type
} Func;

/* Return registers of a function from the ".define __NAME_ret N" that tcc
   writes before its label (see gfunc_prolog in 816-gen.c); all of them when
   the line is missing (older tcc, hand-written assembler). */
static regset declaredReturn(const Func *c)
{
    char key[300];
    snprintf(key, sizeof(key), ".define __%s_ret ", c->name);
    for (int k = 0; k < c->n; k++) {
        if (!startWith(c->f[k].text, key))
            continue;
        switch (atoi(c->f[k].text + strlen(key))) {
        case 0:
            return 0;
        case 1:
            return slotBits("r0");
        case 2:
            return slotBits("r0 r0h");
        case 3:
            return slotBits("r0 r0h r1 r1h");
        case 4:
            return slotBits("f0 f0h");
        default:
            return RETREGS;
        }
    }
    return RETREGS;
}

static int isCallTo(const Line *l, const char *name)
{
    return l->isInsn && (is(l, "jsl") || (is(l, "jsr") && l->size == 'l')) && strcmp(l->op, name) == 0;
}

static int analyseFunc(Func *fn)
{
    curRetLive = fn->retLive;
    fn->ok = fn->n > 0 && analyse(fn->f, fn->n);
    return fn->ok;
}

/* One function: array accesses and dead code, until nothing changes */
static void optimizeFunc(Func *fn, size_t *total, size_t *totalIdx)
{
    while (fn->ok) {
        DBG("-- round %s", fn->name);
        /* order: rewrites, then dead code, then available values (which
           change the patterns the rewrites look for) */
        int x = indexArrays(fn->f, fn->n);
        x += unrollShifts(fn->f, fn->n);
        x += fieldOffsets(fn->f, fn->n);
        x += byteOps(fn->f, fn->n);
        x += unsignedCompares(fn->f, fn->n);
        x += byteIncrements(fn->f, fn->n);
        int r = x ? 0 : removeDead(fn->f, fn->n);
        if (!x && !r)
            x = redundantValues(fn->f, fn->n);
        if (!r && !x)
            break;
        *total += r;
        *totalIdx += x;
        /* rebuild with the insertions and without the removed lines */
        int ins = 0;
        for (int k = 0; k < fn->n; k++)
            if (fn->f[k].insBefore) // one or more lines separated by \n
                for (const char *c = fn->f[k].insBefore; c; c = strchr(c + 1, '\n'))
                    ins++;
        Line *g = calloc(fn->n + ins + 1, sizeof(Line));
        if (!g)
            fatal("malloc-lines");
        int w = 0;
        for (int k = 0; k < fn->n; k++) {
            Line *l = &fn->f[k];
            if (l->insBefore) {
                for (char *c = strtok(l->insBefore, "\n"); c; c = strtok(NULL, "\n")) {
                    char *t = strdup(c);
                    if (!t)
                        fatal("malloc-lines");
                    parseLine(&g[w++], t);
                }
                l->insBefore = NULL;
            }
            if (!l->deleted)
                g[w++] = *l;
            else if (l->anon[0] && !l->dropLabel) {
                /* keep the anonymous label defined on the removed line */
                char *label = strdup(l->anon);
                if (!label)
                    fatal("malloc-lines");
                parseLine(&g[w++], label);
            }
        }
        free(fn->f);
        fn->f = g;
        fn->n = w;
        analyseFunc(fn);
    }
}

dynArray flowOptimize(dynArray file, size_t quiet)
{
    dynArray out;
    size_t total = 0, totalIdx = 0;
    Func *fn = NULL;
    int nfn = 0;

    /* 1. code sections */
    for (size_t i = 0; i < file.used; i++) {
        if (!isCodeSection(file.arr[i]))
            continue;
        size_t start = i + 1, e = start;
        while (e < file.used && !startWith(file.arr[e], ".ENDS"))
            e++;
        Func *t = realloc(fn, (nfn + 1) * sizeof(Func));
        if (!t)
            fatal("malloc-lines");
        fn = t;
        Func *c = &fn[nfn++];
        memset(c, 0, sizeof(*c));
        c->hdr = i;
        c->end = e;
        c->n = (int) (e - start);
        c->f = calloc(c->n > 0 ? c->n : 1, sizeof(Line));
        if (!c->f)
            fatal("malloc-lines");
        for (int k = 0; k < c->n; k++) {
            parseLine(&c->f[k], file.arr[start + k]);
            if (!c->name[0] && c->f[k].isLabel) {
                size_t len = strlen(c->f[k].text) - 1;
                if (len < sizeof(c->name)) {
                    memcpy(c->name, c->f[k].text, len);
                    c->name[len] = 0;
                }
            }
        }
        c->isStatic = startWith(c->name, "tccs_");
        c->retLive = c->retMax = declaredReturn(c);
        i = e;
    }

    /* 2. static functions whose address is never taken */
    for (int k = 0; k < nfn; k++) {
        if (!fn[k].isStatic)
            continue;
        size_t len = strlen(fn[k].name);
        for (size_t i = 0; i < file.used && !fn[k].addrTaken; i++) {
            const char *p = file.arr[i];
            while ((p = strstr(p, fn[k].name)) != NULL) {
                const char *line = file.arr[i];
                char after = p[len];
                int token = (p == line || !(isalnum((unsigned char) p[-1]) || p[-1] == '_'))
                            && !(isalnum((unsigned char) after) || after == '_');
                if (token) {
                    int def = (p == line && after == ':' && p[len + 1] == 0);
                    int call = (startWith(line, "jsr.l ") || startWith(line, "jsl ")) && after == 0
                               && p == line + (line[1] == 's' && line[2] == 'l' ? 4 : 6);
                    if (!def && !call) {
                        fn[k].addrTaken = 1;
                        break;
                    }
                }
                p += len;
            }
        }
    }

    /* 3. analysis, then return registers of the static functions from the
       liveness after their calls. Least fixpoint: start with no return
       register live and grow until stable, so that a recursive call does not
       keep its own return registers alive. A static function is eligible when
       its address is never taken and all its callers are analysed. */
    for (int k = 0; k < nfn; k++)
        if (!analyseFunc(&fn[k]) && fn[k].n > 0)
            DBG("section non analysee: %s", file.arr[fn[k].hdr]);
    int *eligible = calloc(nfn ? nfn : 1, sizeof(int)), neligible = 0;
    for (int k = 0; k < nfn; k++) {
        if (!fn[k].isStatic || fn[k].addrTaken || !fn[k].ok)
            continue;
        eligible[k] = 1;
        for (int g = 0; g < nfn && eligible[k]; g++)
            for (int i = 0; i < fn[g].n; i++)
                if (isCallTo(&fn[g].f[i], fn[k].name) && !fn[g].ok) {
                    eligible[k] = 0;
                    break;
                }
        if (eligible[k]) {
            fn[k].retLive = 0;
            neligible++;
        }
    }
    if (neligible) {
        int stable = 0;
        for (int k = 0; k < nfn; k++)
            if (eligible[k])
                analyseFunc(&fn[k]);
        for (int round = 0; round < 64 && !stable; round++) {
            stable = 1;
            for (int k = 0; k < nfn; k++) {
                if (!eligible[k])
                    continue;
                regset live = fn[k].retLive;
                for (int g = 0; g < nfn; g++)
                    for (int i = 0; i < fn[g].n; i++)
                        if (isCallTo(&fn[g].f[i], fn[k].name))
                            live |= fn[g].f[i].out & fn[k].retMax;
                if (live != fn[k].retLive) {
                    DBG("retour %s : %llx -> %llx", fn[k].name, fn[k].retLive, live);
                    fn[k].retLive = live;
                    stable = 0;
                }
            }
            if (!stable)
                for (int k = 0; k < nfn; k++)
                    analyseFunc(&fn[k]);
        }
        if (!stable) { // no fixpoint reached: back to the safe assumption
            for (int k = 0; k < nfn; k++)
                if (eligible[k])
                    fn[k].retLive = fn[k].retMax;
            for (int k = 0; k < nfn; k++)
                analyseFunc(&fn[k]);
        }
    }
    free(eligible);

    /* 4. optimization of each function */
    for (int k = 0; k < nfn; k++) {
        curRetLive = fn[k].retLive;
        optimizeFunc(&fn[k], &total, &totalIdx);
    }

    /* 5. output */
    size_t cap = file.used + 16;
    for (int k = 0; k < nfn; k++)
        cap += fn[k].n;
    if ((out.arr = malloc(cap * sizeof(char *))) == NULL)
        fatal("malloc-lines");
    out.used = 0;
    int k = 0;
    for (size_t i = 0; i < file.used; i++) {
        out = pushToArray(out, file.arr[i]);
        if (k < nfn && fn[k].hdr == i) {
            for (int j = 0; j < fn[k].n; j++)
                out = pushToArray(out, fn[k].f[j].text);
            i = fn[k].end - 1; // .ENDS is copied next
            free(fn[k].f);
            k++;
        }
    }
    free(fn);

    if (!quiet)
        info("flow analysis: %llu dead instructions removed, %llu array accesses indexed",
             (unsigned long long) total, (unsigned long long) totalIdx);
    return out;
}
