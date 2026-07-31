/*---------------------------------------------------------------------------------

	Copyright (C) 2026-2026
		Alekmaul 

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.
    
        A "romusage"-style ROM/RAM usage report tool for PVSnesLib projects, 
        based on the .sym symbol file produced by  wla-dx's linker (wlalink).

    Inspired by https://github.com/bbbbbr/romusage (Game Boy / GBDK)
    Adapted for the SNES memory map (LoROM / HiROM 
        https://en.wikibooks.org/wiki/Super_NES_Programming/SNES_memory_map)
 
***************************************************************************/

#include "symparse.h"

//-------------------------------------------------------------------------------------------------
t_synbol *g_symbols = NULL;
size_t  g_symbol_count = 0;
size_t  g_symbol_cap = 0;

t_section *g_sections = NULL;
size_t   g_section_count = 0;
size_t   g_section_cap = 0;

t_opensection *g_open = NULL;
size_t g_open_count = 0;
size_t g_open_cap = 0;

t_reportrow *g_rows = NULL;
size_t g_row_count = 0, g_row_cap = 0;

//-------------------------------------------------------------------------------------------------
static void symbol_push(unsigned int addr, const char *name) {
    if (g_symbol_count == g_symbol_cap) {
        g_symbol_cap = g_symbol_cap ? g_symbol_cap * 2 : 4096;
        g_symbols = realloc(g_symbols, g_symbol_cap * sizeof(t_synbol));
        if (!g_symbols) { fatal("out of memory for symbols");  }
    }
    g_symbols[g_symbol_count].addr = addr & 0xFFFFFFu;
    strncpy(g_symbols[g_symbol_count].name, name, MAX_LABEL_LEN - 1);
    g_symbols[g_symbol_count].name[MAX_LABEL_LEN - 1] = '\0';
    g_symbol_count++;
}

static void section_push(const char *name, unsigned int start, unsigned int end) {
    if (g_section_count == g_section_cap) {
        g_section_cap = g_section_cap ? g_section_cap * 2 : 1024;
        g_sections = realloc(g_sections, g_section_cap * sizeof(t_section));
        if (!g_sections) { fatal("out of memory for sections");  }
    }
    t_section *s = &g_sections[g_section_count++];
    strncpy(s->name, name, MAX_LABEL_LEN - 1);
    s->name[MAX_LABEL_LEN - 1] = '\0';
    s->start_addr = start;
    s->end_addr = end;
    s->size = (end >= start) ? (end - start) : 0;
    s->type = MEM_UNKNOWN;
    s->bank = -1;
}

static void open_push(const char *key, unsigned int addr) {
    if (g_open_count == g_open_cap) {
        g_open_cap = g_open_cap ? g_open_cap * 2 : 256;
        g_open = realloc(g_open, g_open_cap * sizeof(t_opensection));
        if (!g_open) { fatal("out of memory for open sections");  }
    }
    strncpy(g_open[g_open_count].key, key, MAX_LABEL_LEN - 1);
    g_open[g_open_count].key[MAX_LABEL_LEN - 1] = '\0';
    g_open[g_open_count].start_addr = addr;
    g_open_count++;
}

// find most recent open section matching key (LIFO), remove it, return start addr.
static int open_pop_matching(const char *key, unsigned int *out_start) {
    for (size_t i = g_open_count; i-- > 0; ) {
        if (strcmp(g_open[i].key, key) == 0) {
            *out_start = g_open[i].start_addr;
            /* remove by shifting down (keeps relative order of the rest, cheap enough) */
            memmove(&g_open[i], &g_open[i + 1], (g_open_count - i - 1) * sizeof(t_opensection));
            g_open_count--;
            return 1;
        }
    }
    return 0;
}

static unsigned int rom_bank_capacity(RomMode mode, int pb) {
    if (mode == MODE_LOROM) {
        if (pb == 0xFE || pb == 0xFF) return 0x8000;
        if (pb >= 0x70 && pb <= 0x7D) return 0x8000;
        if (pb >= 0x40 && pb <= 0x6F) return 0x10000;
        if (pb <= 0x3F) return 0x8000;
        return 0;
    } else {
        if (pb <= 0x3F) return 0x10000;
        return 0;
    }
}

static void row_push(const char *label, const char *type, unsigned int lo, unsigned int hi, unsigned int size, unsigned int used) {
    if (g_row_count == g_row_cap) {
        g_row_cap = g_row_cap ? g_row_cap * 2 : 32;
        g_rows = realloc(g_rows, g_row_cap * sizeof(t_reportrow));
    }
    t_reportrow *r = &g_rows[g_row_count++];
    snprintf(r->label, sizeof(r->label), "%s", label);
    snprintf(r->type, sizeof(r->type), "%s", type);
    r->range_lo = lo; r->range_hi = hi; r->size = size; r->used = used;
}

static int cmp_section_size_desc(const void *a, const void *b) {
    const t_section *sa = *(t_section * const *)a;
    const t_section *sb = *(t_section * const *)b;
    if (sb->size != sa->size) return (sb->size > sa->size) ? 1 : -1;
    return 0;
}

//-------------------------------------------------------------------------------------------------
// .sym file parsing
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

// Parses one non-comment, non-empty line. "AABBCC label" (no$sns flat 24-bit hex, wla-dx default .sym)
//  Returns 1 and fills *addr/name on success, 0 if the line isn't a symbol line.
static int parse_symbol_line(char *line, unsigned int *addr, char *name_out) {
    unsigned int flat = 0;
    char tok[64];
    int n = 0;

    char *s = trim(line);
    if (*s == '\0' || *s == ';' || *s == '[') return 0;

    while (s[n] && !isspace((unsigned char)s[n]) && n < (int)sizeof(tok) - 1) { tok[n] = s[n]; n++; }
    tok[n] = '\0';
    if (n == 0) return 0;
    while (s[n] && isspace((unsigned char)s[n])) n++;
    char *label = s + n;
    if (*label == '\0') return 0;

    // strip any trailing comment / extra columns after the label 
    char *end = label;
    while (*end && !isspace((unsigned char)*end)) end++;
    *end = '\0';

    // must look like plain hex 
    for (char *p = tok; *p; p++) if (!isxdigit((unsigned char)*p)) return 0;
    if (sscanf(tok, "%x", &flat) != 1) return 0;
    *addr = flat & 0xFFFFFFu;

    strncpy(name_out, label, MAX_LABEL_LEN - 1);
    name_out[MAX_LABEL_LEN - 1] = '\0';
    return 1;
}

//-------------------------------------------------------------------------------------------------
void load_sym_file(const char *path, bool isquiet)
{
    char line[2048];
    unsigned int addr;
    char name[MAX_LABEL_LEN];

    FILE *f = fopen(path, "r");
    if (!f) { fatal("cannot open '%s': %s", path, strerror(errno)); }

    if (!isquiet) info("parse sym file [%s]...",path);

    while (fgets(line, sizeof(line), f)) {
        if (parse_symbol_line(line, &addr, name)) {
            symbol_push(addr, name);
        }
    }
    fclose(f);
}

//-------------------------------------------------------------------------------------------------
// Building sections out of SECTIONSTART_/SECTIONEND_ pairs
void build_sections(bool isquiet) 
{
    if (!isquiet) info("bluid sections for %d symbols...",g_symbol_count);

    for (size_t i = 0; i < g_symbol_count; i++) {
        const char *nm = g_symbols[i].name;
        if (strncmp(nm, PFX_START, strlen(PFX_START)) == 0) {
            open_push(nm + strlen(PFX_START), g_symbols[i].addr);
        } else if (strncmp(nm, PFX_END, strlen(PFX_END)) == 0) {
            unsigned int start;
            const char *key = nm + strlen(PFX_END);
            if (open_pop_matching(key, &start)) {
                section_push(key, start, g_symbols[i].addr);
            } else {
                warning("SECTIONEND_%s has no matching SECTIONSTART_ (ignored)", key);
            }
        }
    }
    if (g_open_count > 0) {
        for (size_t i = 0; i < g_open_count; i++)
            warning("SECTIONSTART_%s was never closed (ignored)", g_open[i].key);
    }
}

//-------------------------------------------------------------------------------------------------
// SNES memory map classification                                      
// Classifies one section by its start address (a section never crosses a bank boundary in practice
//  for wla-dx/pvsneslib output; if it did,  we still charge the whole size to the bank it starts in). 
/* $0000-$1FFF of every "system" bank is hard-wired to shadow the first
 * 8KB of WRAM (bank $7E) - see the "LowRAM" rows of the LoROM/HiROM memory
 * maps at https://en.wikibooks.org/wiki/Super_NES_Programming/SNES_memory_map
 *   LoROM : banks $00-$3F (mirrored at $80-$BF)
 *   HiROM : banks $00-$1F (mirrored at $80-$9F)
 * pvsneslib/wla-dx routinely places direct-page variables there (a
 * dedicated MEMORYMAP SLOT of its own), so it's reported as its own
 * LORAM row rather than lumped in with WRAM. */
static int is_loram_bank(RomMode mode, unsigned bank) {
    if (mode == MODE_LOROM) return (bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF);
    return (bank <= 0x1F) || (bank >= 0x80 && bank <= 0x9F); /* HiROM / ExHiROM */
}

static void classify_section(t_section *s, RomMode mode) 
{
    unsigned bank = (s->start_addr >> 16) & 0xFF;
    unsigned off  = s->start_addr & 0xFFFF;

    if ((bank == 0x7E) || (bank == 0x7F)) {
        s->type = MEM_WRAM;
        s->bank = (int)bank;
        return;
    }

    if (off < 0x2000 && is_loram_bank(mode, bank)) {
        s->type = MEM_LORAM;
        s->bank = -1; /* single aggregated pool, not a distinct physical bank */
        return;
    }

    if (mode == MODE_LOROM) {
        if (bank == 0xFE || bank == 0xFF) {
            s->type = (off < 0x8000) ? MEM_SRAM : MEM_ROM;
            s->bank = (int)bank;
            return;
        }
        unsigned pb = (bank >= 0x80 && bank <= 0xFD) ? (bank - 0x80) : bank;
        if (pb <= 0x7D) {
            if (pb >= 0x70 && pb <= 0x7D) {
                s->type = (off < 0x8000) ? MEM_SRAM : MEM_ROM;
                s->bank = (int)pb;
                return;
            }
            if (pb <= 0x3F) {
                if (off < 0x8000) { s->type = MEM_UNKNOWN; s->bank = (int)pb; return; } // $2000-$7FFF: hw regs, no user data expected here 
                s->type = MEM_ROM; s->bank = (int)pb; return;
            }
            // 0x40 - 0x6F : whole bank is ROM (assuming no MAD-1 chip) 
            s->type = MEM_ROM; s->bank = (int)pb; return;
        }
        s->type = MEM_UNKNOWN; s->bank = (int)bank; return;
    } else {
        // HiROM / ExHiROM (ExHiROM handled as a HiROM approximation) 
        if (bank >= 0x40 && bank <= 0x7D) { s->type = MEM_ROM; s->bank = (int)(bank - 0x40); return; }
        if (bank >= 0xC0) { s->type = MEM_ROM; s->bank = (int)(bank - 0xC0); return; }
        // bank in 0x00-0x3F or 0x80-0xBF : mirrors 
        unsigned pb = bank & 0x3F;
        int is_sram_capable_bank = ((bank >= 0x20 && bank <= 0x3F) || (bank >= 0xA0 && bank <= 0xBF));
        if (off >= 0x8000) { s->type = MEM_ROM; s->bank = (int)pb; return; }
        if (is_sram_capable_bank && off >= 0x6000 && off < 0x8000) { s->type = MEM_SRAM; s->bank = (int)pb; return; }
        s->type = MEM_UNKNOWN; s->bank = (int)pb; return;
    }
}

//-------------------------------------------------------------------------------------------------
static const char *memtype_name(MemType t) {
    switch (t) {
        case MEM_ROM:  return "ROM";
        case MEM_LORAM: return "LORAM";
        case MEM_WRAM: return "WRAM";
        case MEM_SRAM: return "SRAM";
        default:       return "?";
    }
}
void print_bar(unsigned int used, unsigned int size) {
    int filled = 0;
    if (size > 0) filled = (int)((unsigned int)used * BAR_WIDTH / size);
    if (filled > BAR_WIDTH) filled = BAR_WIDTH;
    fputc('|',stdout);
    for (int i = 0; i < BAR_WIDTH; i++) fputc(i < filled ? '#' : '.',stdout);
    fputc('|',stdout);
}

void print_row(const t_reportrow *r) {
    unsigned int free_b = (r->size >= r->used) ? (r->size - r->used) : 0;
    double used_pct = r->size ? (100.0 * r->used / r->size) : 0.0;
    double free_pct = r->size ? (100.0 * free_b / r->size) : 0.0;
    char range[24];
    snprintf(range, sizeof(range), "0x%04X-0x%04X", r->range_lo, r->range_hi);
    fprintf(stdout,"%-14s %-6s %-13s %7u %7u %5.0f%% %7u %5.0f%%  ", r->label, r->type, range,
           r->size, r->used, used_pct, free_b, free_pct);
    print_bar(r->used, r->size);
    fputc('\n',stdout);
}

//-------------------------------------------------------------------------------------------------
// display results
void display_symbols(int forcemode, RomMode moderom, int topsection, bool showsection, bool isquiet)
{
    RomMode mode = moderom;
    unsigned int rom_used[256]; 
    int rom_bank_seen[256]; 
    unsigned int wram_used_7e = 0, wram_used_7f = 0;
    unsigned int sram_used_total = 0;
    unsigned int loram_used_total = 0;
    unsigned int unknown_used_total = 0;
    int max_bank = -1;
    unsigned int tot_rom_size = 0, tot_rom_used = 0;
    unsigned int tot_wram_size = 0, tot_wram_used = 0;
    unsigned int tot_sram_size = 0, tot_sram_used = 0;
    unsigned int tot_loram_size = 0, tot_loram_used = 0;

    memset(rom_used, 0, sizeof(rom_used));
    memset(rom_bank_seen, 0, sizeof(rom_bank_seen));

    // auto-detect LoROM vs HiROM. $0000-$1FFF of a system bank is always a  WRAM mirror in both modes, 
    //   so it proves nothing either way. But a  non-WRAM section sitting in $2000-$7FFF of a system bank can only
    //   happen under HiROM (that range has no ROM mapping at all in LoROM). 
    if (!forcemode) {
        int hirom_guess = 0;
        for (int i = 0; i < g_section_count; i++) {
            unsigned bank = (g_sections[i].start_addr >> 16) & 0xFF;
            unsigned off  = g_sections[i].start_addr & 0xFFFF;
            if (bank == 0x7E || bank == 0x7F) continue;
            int in_mirror_range = (bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF);
            if (in_mirror_range && off >= 0x2000 && off < 0x8000) { hirom_guess = 1; break; }
        }
        mode = hirom_guess ? MODE_HIROM : MODE_LOROM;
    }

    for (int i = 0; i < g_section_count; i++) classify_section(&g_sections[i], mode);

    // aggregate ROM usage per physical bank, WRAM per bank (7E/7F), SRAM as one pool 
    for (int i = 0; i < g_section_count; i++) {
        t_section *s = &g_sections[i];
        switch (s->type) {
            case MEM_ROM:
                if (s->bank >= 0 && s->bank < 256) { rom_used[s->bank] += s->size; rom_bank_seen[s->bank] = 1; }
                break;
            case MEM_WRAM:
                if (s->bank == 0x7E) wram_used_7e += s->size;
                else if (s->bank == 0x7F) wram_used_7f += s->size;
                break;
            case MEM_LORAM:
                loram_used_total += s->size;
                break;
            case MEM_SRAM:
                sram_used_total += s->size;
                break;
            default:
                unknown_used_total += s->size;
                break;
        }
    }

    for (int b = 0; b < 256; b++) if (rom_bank_seen[b]) max_bank = b;

    // build report rows: ROM banks 
    for (int b = 0; b <= max_bank; b++) {
        unsigned int cap = rom_bank_capacity(mode, b);
        if (cap == 0) continue; // not a valid ROM bank in this mode (e.g. 0x7E/0x7F skipped already) 
        if (!rom_bank_seen[b]) continue; // skip unused banks unless explicitly requested 
        char label[40];
        snprintf(label, sizeof(label), "ROM bank $%02X", b);
        unsigned int lo = (mode == MODE_LOROM && b >= 0x40 && b <= 0x6F) ? 0x0000 : (mode == MODE_HIROM) ? 0x0000 : 0x8000;
        unsigned int hi = 0xFFFF;
        row_push(label, "ROM", lo, hi, cap, rom_used[b]);
    }
    // FE/FF LoROM ROM extension banks, only if used 
    if (mode == MODE_LOROM) {
        if (rom_bank_seen[0xFE]) row_push("ROM bank $FE", "ROM", 0x8000, 0xFFFF, rom_bank_capacity(mode, 0xFE), rom_used[0xFE]);
        if (rom_bank_seen[0xFF]) row_push("ROM bank $FF", "ROM", 0x8000, 0xFFFF, rom_bank_capacity(mode, 0xFF), rom_used[0xFF]);
    }

    // WRAM: fixed hardware fact, independent of mapping mode 
    row_push("WRAM bank $7E", "WRAM", 0x0000, 0xFFFF, 0x10000, wram_used_7e);
    row_push("WRAM bank $7F", "WRAM", 0x0000, 0xFFFF, 0x10000, wram_used_7f);

    /* LowRAM: the $0000-$1FFF shadow of bank $7E present in every system
     * bank (see is_loram_bank()). Reported as its own row/type since
     * pvsneslib/wla-dx treat it as a distinct MEMORYMAP slot (typically
     * used for direct-page variables), even though physically it's the
     * same chip as WRAM bank $7E. */
    row_push("LowRAM ($7E)", "LORAM", 0x0000, 0x1FFF, 0x2000, loram_used_total);
    
    // SRAM:  auto-sized to the next convenient size that fits what's actually used 
    if (sram_used_total > 0) {
        unsigned int cap;
        cap = 2048; // SNES header SRAM sizes are typically 2/8/32/... KB; start at 2KB 
        while (cap < sram_used_total) cap *= 2;
        row_push("Cartridge SRAM", "SRAM", 0x0000, cap ? cap - 1 : 0, cap, sram_used_total);
    }

    fprintf(stdout,"Memory map          : %s%s\n",
            mode == MODE_LOROM ? "LoROM" : mode == MODE_HIROM ? "HiROM" : "ExHiROM (approx.)",
            moderom ? "" : "  (auto-detected)");
    fprintf(stdout,"Sections parsed     : %zu\n\n", g_section_count);

    fprintf(stdout,"%-14s %-6s %-13s %7s %7s %6s %7s %6s\n", "Bank", "Type", "Range", "Size", "Used", "Used%", "Free", "Free%");
    fprintf(stdout,"-------------- ------ ------------- ------- ------- ------ ------- ------\n");

    for (int i = 0; i < g_row_count; i++) {
        print_row(&g_rows[i]);
        if (!strcmp(g_rows[i].type, "ROM"))  { tot_rom_size += g_rows[i].size; tot_rom_used += g_rows[i].used; }
        if (!strcmp(g_rows[i].type, "WRAM")) { tot_wram_size += g_rows[i].size; tot_wram_used += g_rows[i].used; }
        if (!strcmp(g_rows[i].type, "LORAM")) { tot_loram_size += g_rows[i].size; tot_loram_used += g_rows[i].used; }
        if (!strcmp(g_rows[i].type, "SRAM")) { tot_sram_size += g_rows[i].size; tot_sram_used += g_rows[i].used; }
    }

    fprintf(stdout,"-------------- ------ ------------- ------- ------- ------ ------- ------\n");
    if (tot_rom_size) {
        row_push("TOTAL ROM", "ROM", 0, 0, tot_rom_size, tot_rom_used);
        print_row(&g_rows[g_row_count - 1]);
    }
    if (tot_wram_size) {
        row_push("TOTAL WRAM", "WRAM", 0, 0, tot_wram_size, tot_wram_used);
        print_row(&g_rows[g_row_count - 1]);
    }
    if (tot_loram_size) {
        row_push("TOTAL LORAM", "LORAM", 0, 0, tot_loram_size, tot_loram_used);
        print_row(&g_rows[g_row_count - 1]);
    }
    if (tot_sram_size) {
        row_push("TOTAL SRAM", "SRAM", 0, 0, tot_sram_size, tot_sram_used);
        print_row(&g_rows[g_row_count - 1]);
    }
    fprintf(stdout," \n");

    if (unknown_used_total > 0) {
        printf("\nnote: %u bytes were placed in addresses this tool doesn't classify\n"
                    "      as ROM/WRAM/SRAM for %s (hardware register / mirrored area).\n"
                    "      Run with --sections to see exactly which sections these are.\n",
                unknown_used_total, mode == MODE_LOROM ? "LoROM" : "HiROM");
    }

    // display section if required
    if (showsection) {
        fprintf(stdout,"%-34s %-6s %-8s %8s\n", "t_section", "Type", "Bank", "Size");
        for (int i = 0; i < g_section_count; i++) {
            t_section *s = &g_sections[i];
            fprintf(stdout, "%-34s %-6s $%02X      %8u\n", s->name, memtype_name(s->type),
                s->bank >= 0 ? s->bank : (int)((s->start_addr >> 16) & 0xFF), s->size);
        }
        fprintf(stdout,"\n");
    }

    if (topsection > 0) {
        t_section **ptrs = malloc(g_section_count * sizeof(t_section *));
        for (size_t i = 0; i < g_section_count; i++) ptrs[i] = &g_sections[i];
        qsort(ptrs, g_section_count, sizeof(t_section *), cmp_section_size_desc);
        int n = topsection < (int)g_section_count ? topsection : (int)g_section_count;
        fprintf(stdout,"\nTop %d largest sections:\n", n);
        fprintf(stdout,"%-34s %-6s %-8s %8s\n", "t_section", "Type", "Bank", "Size");
        for (int i = 0; i < n; i++) {
            t_section *s = ptrs[i];
            fprintf(stdout,"%-34s %-6s $%02X      %8u\n", s->name, memtype_name(s->type),
                    s->bank >= 0 ? s->bank : (int)((s->start_addr >> 16) & 0xFF), s->size);
        }
        free(ptrs);
    }
}
