/*
 *  WDC 65816 code generator for TCC
 * 
 *  Copyright (c) 2007 Ulrich Hecht
 *
 *  Based on arm-gen.c by Daniel Glöckner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define LDOUBLE_SIZE 12 // not actually supported
#define LDOUBLE_ALIGN 4
#define MAX_ALIGN 8

#define NB_REGS 15

#define RC_INT 0x0001
#define RC_FLOAT 0x0002
#define RC_R0 0x0004
#define RC_R1 0x0008
#define RC_R2 0x0010
#define RC_R3 0x0020
#define RC_R4 0x0040
#define RC_R5 0x0080
#define RC_R9 0x0100
#define RC_R10 0x0200
#define RC_F0 0x0400
#define RC_F1 0x0800
#define RC_F2 0x1000
#define RC_F3 0x2000
#define RC_NONE 0x8000

#define RC_IRET RC_R0
#define RC_LRET RC_R1
#define RC_FRET RC_F0

enum {
  TREG_R0 = 0,
  TREG_R1,
  TREG_R2,
  TREG_R3,
  TREG_R4,
  TREG_R5,
  TREG_R9 = 9,
  TREG_R10,
  TREG_F0,
  TREG_F1,
  TREG_F2,
  TREG_F3,
};

int reg_classes[NB_REGS] = {
  RC_INT | RC_R0,
  RC_INT | RC_R1,
  RC_INT | RC_R2,
  RC_INT | RC_R3,
  RC_INT | RC_R4,
  RC_INT | RC_R5,
  RC_NONE,
  RC_NONE,
  RC_NONE,
  RC_R9,
  RC_R10,
  RC_FLOAT | RC_F0,
  RC_FLOAT | RC_F1,
  RC_FLOAT | RC_F2,
  RC_FLOAT | RC_F3,
};

#define REG_IRET TREG_R0
#define REG_LRET TREG_R1
#define REG_FRET TREG_F0

#define R_DATA_32 1 // whatever
#define R_JMP_SLOT 2 // whatever
#define R_COPY 3 // whatever

#define ELF_PAGE_SIZE 0x1000 // whatever
#define ELF_START_ADDR 0x400 // made up

#define PTR_SIZE 4

#define EM_TCC_TARGET EM_W65

#define LOCAL_LABEL "__local_%d"

char current_fn[256] = "";

/* yet another terrible workaround
   WLA does not have file-local symbols, only section-local and global.
   thus, everything that is file-local must be made global and given a
   unique name. not knowing how to choose one deterministically (filename?
   could be more than one file with the same name...), I chose to use a
   random number, saved to static_prefix. */
//char* static_prefix = "__tccs_";
/* with WLA 9.X, if you have a label that begins with a _ and it is inside a section,
   then the name doesn't show outside of that section.
   If it is not inside a section it doesn't show outside of the object file...
*/
char* static_prefix = "tccs_";

char* label_workaround = NULL;
struct labels_816 {
  char* name;
  int pos;
};
struct labels_816 label[1000];
int labels = 0;

char* get_sym_str(Sym* sym)
{
  static char name[256];
  char* symname;
  
  symname = get_tok_str(sym->v, NULL);
  
  name[0] = 0;
  /* if static, add prefix */
  if(sym->type.t & VT_STATIC) {
    //fprintf(stderr,"sym %s type 0x%x current_fn %s token %d\n",symname,sym->type.t,current_fn,sym->v);
    if((sym->type.t & VT_STATICLOCAL) && current_fn[0] != 0 && !((sym->type.t & VT_BTYPE) == VT_FUNC))
      sprintf(name, "%s_FUNC_%s_", static_prefix, current_fn);
    else
      strcpy(name, static_prefix);
  }
    
  /* add symbol name */
  strcat(name, symname);

  //fprintf(stderr,"symbol %s type 0x%x\n", name, sym->type.t);
  return name;
}

/* XXX: make it faster ? */
void g(int c)
{
    int ind1;
    ind1 = ind + 1;
    if (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

void s(char* str)
{
  for(;*str;str++) g(*str);
}

char line[256];
#define pr(x...) do { sprintf(line, x); s(line); } while(0)

int jump[1000][2];
int jumps = 0;


void gsym_addr(int t, int a)
{
  /* code at t wants to jump to a */
  //fprintf(stderr, "gsymming t 0x%x a 0x%x\n", t, a);
  pr("; gsym_addr t %d a %d ind %d\n",t,a,ind);
  /* the label generation code sets this for us so we know when a symbol
     is a label and what its name is, so that we can remember its name
     and position so the output code can insert it correctly */
  if(label_workaround) {
    //fprintf("setting label %s to a %d (t %d)\n", label_workaround, a, t);
    label[labels].name = label_workaround;
    label[labels].pos = a;
    labels++;
    label_workaround = NULL;
  }
  int i;
  // pair up the jump with the target address
  // the tcc_output_... function will add a
  // label __local_<i> at a when writing the output
  int found = 0;
  for(i = 0; i < jumps; i++) {
    if(jump[i][0] == t) jump[i][1] = a;
    found = 1;
  }
  if(!found) pr("; ERROR no jump found to patch\n");
}

void gsym(int t)
{
  gsym_addr(t,ind);
}

int stack_back = 0;
int adjust_stack(int fc, int disp)
{
  pr("; stack adjust: fc + disp - loc %d\n", fc + disp - loc);
  if(fc + disp - loc < 256) { stack_back = 0; return fc; }
  stack_back = -loc + fc;
  pr("tsc\nclc\nadc.w #%d\ntcs\n", stack_back);
  return fc - stack_back;
}

int restore_stack(int fc)
{
  if(!stack_back) return fc;
  pr("tsc\nsec\nsbc.w #%d\ntcs\n", stack_back);
  return fc + stack_back;
}

// this used to be local to gfunc_call, but we need it to get the correct
// stack pointer displacement while building the argument stack for
// a function call
int args_size = 0;

int ll_workaround = 0;

void load(int r, SValue* sv)
{
  int fr,ft,fc;
  int length;
  int align;
  int v,sign,t;
  SValue v1;
  pr("; load %d\n",r);
  pr("; type %d reg 0x%x extra 0x%x\n",sv->type.t,sv->r,sv->type.extra);
  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.ul;
  
  length = type_size(&sv->type, &align);
  if((ft & VT_BTYPE) == VT_LLONG) length = 2; // long longs are handled word-wise
  if(ll_workaround) length = 4;

  //pr("; load r 0x%x fr 0x%x ft 0x%x fc 0x%x\n",r,fr,ft,fc);

  int base = -1;
  v = fr & VT_VALMASK;
  if(fr & VT_LVAL) {
    if(v == VT_LLOCAL) {
      v1.type.t = VT_PTR;
      v1.r = VT_LOCAL | VT_LVAL;
      v1.c.ul = sv->c.ul;
      load(base=10 /* lr */, &v1);
      fc=sign=0;
      v=VT_LOCAL;
    }
    else if(v == VT_CONST) {
      if(fr & VT_SYM) {	// deref symbol + displacement
        char* sy = get_sym_str(sv->sym);
        if(is_float(ft)) {
          pr("; fld%d [%s + %d], tcc__f%d\n", length, sy, fc, r - TREG_F0);
          switch(length) {
          case 4: pr("lda.l %s + %d\nsta.b tcc__f%d\nlda.l %s + %d + 2\nsta.b tcc__f%dh\n", sy, fc, r - TREG_F0, sy, fc, r - TREG_F0); break;
          default: error("ICE 1");
          }
        }
        else {
          pr("; ld%d [%s + %d], tcc__r%d\n", length, sy, fc, r);
          // FIXME: This implementation is moronic
          if(fc > 65535) error("index too big");
          switch(length) {
          case 1:
            pr("lda.w #0\nsep #$20\nlda.l %s + %d\nrep #$20\n", sy, fc);
            if(!(ft & VT_UNSIGNED)) pr("xba\nxba\nbpl +\nora.w #$ff00\n+\n");
            pr("sta.b tcc__r%d\n", r);
            break;
          case 2: pr("lda.l %s + %d\nsta.b tcc__r%d\n", sy, fc, r); break;
          case 4: pr("lda.l %s + %d\nsta.b tcc__r%d\nlda.l %s + %d + 2\nsta.b tcc__r%dh\n", sy, fc, r, sy, fc, r); break;
          default: error("ICE 1");
          }
        }
      }
      else {	// deref constant pointer
        //error("ld [%d],tcc__r%d\n",fc,r);
        pr("; deref constant ptr ld [%d],tcc__r%d\n", fc, r);
        if(is_float(ft)) {
          error("dereferencing constant float pointers unimplemented\n");
        }
        else {
          switch(length) {
          case 1:
            pr("lda.w #0\nsep #$20\nlda.l %d\nrep #$20\n", fc);
            if(!(ft & VT_UNSIGNED)) pr("xba\nxba\nbpl +\nora.w #$ff00\n+\n");
            pr("sta.b tcc__r%d\n", r);
            break;
          case 2: pr("lda.l %d\nsta.b tcc__r%d\n", fc, r); break;
          case 4: pr("lda.l %d\nsta.b tcc__r%d\nlda.l %d + 2\nsta.b tcc__r%dh\n", fc, r, fc, r); break;
          default: error("ICE 1");
          }
        }
      }
      return;
    }
    else if(v < VT_CONST) { // deref pointer in register
      base = v;
      fc = sign = 0;
      v = VT_LOCAL;
    }
    
    if(v == VT_LOCAL) {
      if(is_float(ft)) {
        if(base == -1) {
          pr("; fld%d [sp,%d],tcc__f%d\n", length, fc, r - TREG_F0);
          if(length != 4) error("ICE 2f");
          fc = adjust_stack(fc, args_size + 2);
          pr("lda %d + __%s_locals + 1,s\nsta.b tcc__f%d\nlda %d + __%s_locals + 1,s\nsta.b tcc__f%dh\n", fc+args_size, current_fn, r - TREG_F0, fc+args_size+2, current_fn, r - TREG_F0);
          fc = restore_stack(fc);
        }
        else {
          pr("; fld%d [tcc__r%d,%d],tcc__f%d\n", length, base, fc, r - TREG_F0);
          if(length != 4) error("ICE 3f");
          pr("ldy #%d\nlda.b [tcc__r%d],y\nsta.b tcc__f%d\niny\niny\nlda.b [tcc__r%d], y\nsta.b tcc__f%dh\n", fc, base, r - TREG_F0, base, r - TREG_F0);
        }
      }
      else {
        if(base == -1) {	// value of local at fc
          pr("; ld%d [sp,%d],tcc__r%d\n",length,fc,r);
          fc = adjust_stack(fc, args_size + 2);
          switch(length) {
            case 1:
              pr("lda.w #0\nsep #$20\nlda %d + __%s_locals + 1,s\nrep #$20\n", fc+args_size, current_fn);
              if(!(ft & VT_UNSIGNED)) pr("xba\nxba\nbpl +\nora.w #$ff00\n+\n");
              pr("sta.b tcc__r%d\n", r);
              break;
            case 2: pr("lda %d + __%s_locals + 1,s\nsta.b tcc__r%d\n", fc+args_size, current_fn, r); break;
            case 4: pr("lda %d + __%s_locals + 1,s\nsta.b tcc__r%d\nlda %d + __%s_locals + 1,s\nsta.b tcc__r%dh\n", fc+args_size, current_fn, r, fc+args_size + 2, current_fn, r); break;
            default: error("ICE 2"); break;
          }
          fc = restore_stack(fc);
        }
        else {	// value of array member r[fc]
          pr("; ld%d [tcc__r%d,%d],tcc__r%d\n",length, base, fc, r);
          switch(length) {
            case 1:
              pr("lda.w #0\n");
              if(!fc) pr("sep #$20\nlda.b [tcc__r%d]\nrep #$20\n", base);
              else pr("ldy #%d\nsep #$20\nlda.b [tcc__r%d],y\nrep #$20\n", fc, base);
              if(!(ft & VT_UNSIGNED)) pr("xba\nxba\nbpl +\nora.w #$ff00\n+\n");
              pr("sta.b tcc__r%d\n", r);
              break;
            case 2:
              if(!fc) pr("lda.b [tcc__r%d]\nsta.b tcc__r%d\n", base, r);
              else pr("ldy #%d\nlda.b [tcc__r%d],y\nsta.b tcc__r%d\n", fc, base, r);
              break;
            case 4: pr("ldy #%d\nlda.b [tcc__r%d],y\nsta.b tcc__r%d\niny\niny\nlda.b [tcc__r%d],y\nsta.b tcc__r%dh\n", fc, base, r, base, r); break;
            default: error("ICE 3"); break;
          }
        }
      }
      return;
    }
  } else {	// VT_LVAL
    if(v == VT_CONST) {
      if(fr & VT_SYM) {	// symbolic constant
        char* sy = get_sym_str(sv->sym);
        pr("; ld%d #%s + %d, tcc__r%d (type 0x%x)\n", length,sy, fc, r, ft);
        if(length != PTR_SIZE) pr("; FISHY! length <> PTR_SIZE! (may be an array)\n");
        pr("lda.w #:%s\nsta.b tcc__r%dh\nlda.w #%s + %d\nsta.b tcc__r%d\n", sy, r, sy, fc, r);
      }
      else {	// numeric constant
        pr("; ld%d #%d,tcc__r%d\n",length,sv->c.ul,r);
        if((ft & VT_BTYPE) == VT_BOOL) {
          sv->c.ul = sv->c.ul? 1: 0;
        }
        switch(length) {
          case 1:
            if (ft & VT_UNSIGNED) {
              pr("lda.w #%d\n", sv->c.ul & 0xff);
            }
            else {
              pr("lda.w #%d\n", ((short)((sv->c.ul & 0xff) << 8)) >> 8);
            }
            pr("sta.b tcc__r%d\n", r);
            break;
          case 2: pr("lda.w #%d\nsta.b tcc__r%d\n", sv->c.ul & 0xffff, r); break;
          case 4: pr("lda.w #%d\nsta.b tcc__r%d\nlda.w #%d\nsta.b tcc__r%dh\n", sv->c.ul & 0xffff, r, sv->c.ul >> 16, r); break;
          default: error("ICE 4");
        }
      }
      return;
    }
    else if(v == VT_LOCAL) {
      if(fr & VT_SYM) {
        error("symbol");
        char* sy = get_sym_str(sv->sym);
        pr("; LOCAL ld%d #%s, tcc__r%d (type 0x%x)\n", length,sy, r, ft);
      }
      else {	// local pointer
        pr("; ld%d #(sp) + %d,tcc__r%d (fr 0x%x ft 0x%x fc 0x%x)\n",length,sv->c.ul,r,fr,ft,fc);
        // pointer; have to ensure the upper word is correct (page 0)
        pr("stz.b tcc__r%dh\ntsa\nclc\nadc #(%d + __%s_locals + 1)\nsta.b tcc__r%d\n", r, sv->c.ul + args_size, current_fn, r);
      }
      return;
    }
    else if(v == VT_CMP) {
      error("cmp");
      return;
    }
    else if(v == VT_JMP || v == VT_JMPI) {
      t = v & 1; // inverted or not
      pr("; jmpr(i) v 0x%x r 0x%x fc 0x%x\n",v,r,fc);
      pr("lda #%d\nbra +\n", t);
      gsym(fc);
      pr("lda #%d\n+\nsta.b tcc__r%d\n", t^1, r);	// stz rXh seems to be unnecessary (we only look at the lower word)
      return;
    }
    else if(v < VT_CONST) {	// register value
      if(is_float(ft)) {
        v -= TREG_F0;
        r -= TREG_F0;
        pr("; fmov tcc__f%d, tcc__f%d\n", v, r);
        pr("lda.b tcc__f%d\nsta.b tcc__f%d\nlda.b tcc__f%dh\nsta.b tcc__f%dh\n", v, r, v, r);
      }
      else {
        pr("; mov tcc__r%d, tcc__r%d\n",v,r);
        pr("lda.b tcc__r%d\nsta.b tcc__r%d\nlda.b tcc__r%dh\nsta.b tcc__r%dh\n", v, r, v, r);
      }
      return;
    }
  }
  error("load unimplemented");
}

void store(int r, SValue* sv)
{
  int v, ft, fc, fr, sign;
  int base;
  int length, align;
  SValue v1;
  
  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.i;
  
  length = type_size(&sv->type, &align);
  if((ft & VT_BTYPE) == VT_LLONG) length = 2; // long longs are handled word-wise
  
  pr("; store r 0x%x fr 0x%x ft 0x%x fc 0x%x\n",r,fr,ft,fc);

  v = fr & VT_VALMASK;
  base = -1;
  if ((fr & VT_LVAL) || fr == VT_LOCAL) {
    if(v < VT_CONST) {	// deref register
      base = v;
      v=VT_LOCAL;
      fc=sign=0; // no clue...
    }
    if(v == VT_CONST) {
      if(fr & VT_SYM) {	// deref symbol
        char* sy = get_sym_str(sv->sym);
        if(r >= TREG_F0) pr("; fst%d tcc__f%d, [%s,%d]\n", length, r - TREG_F0, sy, fc);
        else pr("; st%d tcc__r%d, [%s,%d]\n", length, r, sy, fc);
        if(r >= TREG_F0 && length != 4) error("illegal float store of length %d", length);
        switch(length) {
          case 1: pr("sep #$20\nlda.b tcc__r%d\nsta.l %s + %d\nrep #$20\n", r, sy, fc); break;
          case 2: pr("lda.b tcc__r%d\nsta.l %s + %d\n", r, sy, fc); break;
          case 4: 
            if(r >= TREG_F0)
              pr("lda.b tcc__f%d\nsta.l %s + %d\nlda.b tcc__f%dh\nsta.l %s + %d + 2\n", r - TREG_F0, sy, fc, r - TREG_F0, sy, fc);
            else
              pr("lda.b tcc__r%d\nsta.l %s + %d\nlda.b tcc__r%dh\nsta.l %s + %d + 2\n", r, sy, fc, r, sy, fc);
            break;
          default: error("ICE 5"); break;
        }
        return;
      }
      else {
        v1.type.t = VT_PTR; //ft;
        v1.r = fr & ~VT_LVAL;
        v1.c.ul = sv->c.ul;
        v1.sym = sv->sym;
        load(base=9,&v1);
        fc = sign = 0;
        v = VT_LOCAL;
      }
    }
    if(v == VT_LOCAL) {
      if(r >= TREG_F0) {
        if(base < 0) {
          pr("; fst%d tcc__f%d, [sp,%d]\n", length, r - TREG_F0, fc);
          fc = adjust_stack(fc, args_size + 2);
          switch(length) {
            case 4: pr("lda.b tcc__f%d\nsta %d + __%s_locals + 1,s\nlda.b tcc__f%dh\nsta %d + __%s_locals + 1,s\n", r - TREG_F0, fc+args_size, current_fn, r - TREG_F0, fc+args_size + 2, current_fn); break;
            default: error("ICE 6f"); break;
          }
          fc = restore_stack(fc);
        }
        else {
          pr("; fst%d tcc__f%d, [tcc__r%d,%d]\n", length, r - TREG_F0, base, fc);
          switch(length) {
            case 4: pr("ldy.w #0\nlda.b tcc__f%d\nsta.b [tcc__r%d],y\niny\niny\nlda.b tcc__f%dh\nsta.b [tcc__r%d],y\n", r - TREG_F0, base, r - TREG_F0, base); break;
            default: error("ICE 7f"); break;
          }
        }
	return;
      } else {
        if(base < 0) {	// write to local at fc
          pr("; st%d tcc__r%d, [sp,%d]\n",length,r,fc);
          fc = adjust_stack(fc, args_size + 2);
          switch(length) {
            case 1: pr("sep #$20\nlda.b tcc__r%d\nsta %d + __%s_locals + 1,s\nrep #$20\n", r, fc+args_size, current_fn); break;
            case 2: pr("lda.b tcc__r%d\nsta %d + __%s_locals + 1,s\n", r, fc+args_size, current_fn); break;
            case 4: pr("lda.b tcc__r%d\nsta %d + __%s_locals + 1,s\nlda.b tcc__r%dh\nsta %d + __%s_locals + 1,s\n", r, fc+args_size, current_fn, r, fc+args_size + 2, current_fn); break;
            default: error("ICE 6"); break;
          }
          fc = restore_stack(fc);
        }
        else {		// write to array member r[fc]
          pr("; st%d tcc__r%d, [tcc__r%d,%d]\n",length,r,base,fc);
          switch(length) {
            case 1:
              pr("sep #$20\nlda.b tcc__r%d\n", r);
              if(!fc) pr("sta.b [tcc__r%d]\nrep #$20\n", base);
              else pr("ldy #%d\nsta.b [tcc__r%d],y\nrep #$20\n", fc, base);
              break;
            case 2:
              pr("lda.b tcc__r%d\n", r);
              if(!fc) pr("sta.b [tcc__r%d]\n", base);
              else pr("ldy #%d\nsta.b [tcc__r%d],y\n", fc, base);
              break;
            case 4: pr("lda.b tcc__r%d\nldy #%d\nsta.b [tcc__r%d],y\nlda.b tcc__r%dh\niny\niny\nsta.b [tcc__r%d],y\n", r, fc, base, r, base); break;
            default: error("ICE 7"); break;
          }
        }
      }
      return;
    }
  }
  error("store unimplemented");

}

void gfunc_call(int nb_args)
{
  int align, r, i, func_call;
  Sym *func_sym;
  
  int length;

  /* args_size is the size of the function call arguments already
     pushed on the stack. needed so that loads and stores to
     locals on the stack still work while building an argument
     list. needs to be restored before returning to make
     nested function calls work (passing structs by value causes
     a memcpy call) */
  int restore_args_size = args_size;
  
  for(i = 0;i < nb_args; i++) {

      length = type_size(&vtop->type, &align);
      if(vtop->type.t & VT_ARRAY) length = PTR_SIZE;
      
      if ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
          /* allocate the necessary size on stack */
          pr("; sub sp, #%d\n",length);
          pr("tsa\nsec\nsbc #%d\ntas\n", length);
          args_size += length;
          
          /* generate structure store */
          r = get_reg(RC_INT);
          /* put the pointer to struct store on the stack
             always remember: the 65xxs are off-by-one processors
             there is always something to increment or decrement to
             get the value you actually want. (cf. mvn/mvp) */
          pr("stz.b tcc__r%dh\ntsa\nina\nsta.b tcc__r%d\n",r,r);
          
          /* here, TCC generates a memcpy call
             this recursion makes the restore_args_size stuff necessary */
          vset(&vtop->type, r | VT_LVAL, 0);
          vswap();
          vstore();
      } else if (is_float(vtop->type.t)) {
        if(length != 4) error("unknown float size %d\n", length);
        r = gv(RC_FLOAT);
        pr("; fldpush%d (type 0x%x reg 0x%x) tcc__f%d\n", length, vtop->type.t, vtop->r, r - TREG_F0);
        pr("pei (tcc__f%dh)\npei (tcc__f%d)\n", r - TREG_F0, r - TREG_F0);
        args_size += length;
      } else {
          /* simple type (currently always same size) */
          /* XXX: implicit cast ? */
          if(((vtop->r & VT_VALMASK) == VT_CONST) && ((vtop->r & VT_LVAL) == 0)) {
            // push immediate
            pr("; push%d imm r 0x%x\n",length,vtop->r);
            if(vtop->r & VT_SYM) {
              char* sy = get_sym_str(vtop->sym);
              if(length != PTR_SIZE) pr("; FISHY! length <> PTR_SIZE! (may be an array)\n");
              pr("pea.w :%s\npea.w %s %c %d\n", sy, sy, vtop->c.i < 0 ? '-' : '+', abs(vtop->c.i));
              length = PTR_SIZE;
            }
            else {
              switch(length) {
                case 1: pr("sep #$20\nlda #%d\npha\nrep #$20\n", vtop->c.ul & 0xff); break;
                case 2: pr("pea.w %d\n", vtop->c.ul & 0xffff); break;
                case 4: 
                  pr("pea.w %d\npea.w %d\n", vtop->c.ul >> 16, vtop->c.ul & 0xffff);
                  break;
                default: error("cannot push %d-byte immediate", length); break;
              }
            }
          }
          else {
            // load to register, then push
            pr("; ldpush before load type 0x%x reg 0x%x\n", vtop->type.t, vtop->r);
            r = gv(RC_INT);
            pr("; ldpush%d (type 0x%x reg 0x%x) tcc__r%d\n", length, vtop->type.t, vtop->r, r);
            switch(length) {
              case 1: pr("sep #$20\nlda.b tcc__r%d\npha\nrep #$20\n", r); break;
              //case 2: pr("lda.b tcc__r%d\npha\n", r); break;
              case 2: pr("pei (tcc__r%d)\n", r); break;
              case 4:
                if((vtop->type.t & VT_BTYPE) == VT_LLONG) {
                  //pr("lda.b tcc__r%d\npha\nlda.b tcc__r%d\npha\n", vtop->r2, r);
                  pr("pei (tcc__r%d)\npei (tcc__r%d)\n", vtop->r2, r);
                }
                //else pr("lda.b tcc__r%dh\npha\nlda.b tcc__r%d\npha\n", r, r);
                else pr("pei (tcc__r%dh)\npei (tcc__r%d)\n", r, r);
                break;
              default: error("cannot push %d-byte from register", length); break;
            }
          }
          args_size += length;
      }
      vtop--;
  }
  save_regs(0); /* save used temporary registers */
  func_sym = vtop->type.ref;
  func_call = func_sym->r;

  /* fast call case */
  if (func_call >= FUNC_FASTCALL1 && func_call <= FUNC_FASTCALL3) {
    error("fastcall");
  }

  pr("; call r 0x%x\n",vtop->r);
  if(vtop->r & VT_LVAL) {
    // call a function pointer
    if((vtop->r & VT_VALMASK) == VT_LLOCAL) {
      SValue v1;
      v1.type.t = VT_PTR;
      v1.r = VT_LOCAL | VT_LVAL;
      v1.c.ul = vtop->c.ul;
      load(9, &v1);
      // the 65816 is two stoopid to do a jsl [r10], so we have to jump thru a hoop here
      pr("; eins\njsr.l tcc__jsl_ind_r9\n");
    }
    else {	// call a symbolic function pointer
      pr("; symfpcall vtop->sym %p vtop->r 0x%x vtop->type.t 0x%x c 0x%x\n", vtop->sym, vtop->r, vtop->type.t, vtop->c.ui);
      gv(RC_R10);
      pr("; zwei\njsr.l tcc__jsl_r10\n");
    }
  }
  else
    pr("jsr.l %s\n", get_sym_str(vtop->sym));

  if (args_size - restore_args_size && func_sym->r != FUNC_STDCALL) {
      pr("; add sp, #%d\n",args_size - restore_args_size);
      // pull the arguments off the stack
      if(args_size - restore_args_size == 2) pr("pla\n");
      else pr("tsa\nclc\nadc #%d\ntas\n",args_size - restore_args_size);
  }
  args_size = restore_args_size;
  vtop--;
}

int gjmp(int t)
{
  int r;
  int i;
  // remember this jump so we can insert a label before the destination later
  pr("; gjmp_addr %d at %d\n",t,ind);
  pr("jmp.w " LOCAL_LABEL "\n",jumps);
  r = ind;
  jump[jumps][0] = r;
  for(i = 0; i < jumps; i++) {
    if(jump[i][0] == t) {	// the jump target is a jump itself; make it go to same place as this one
      jump[i][0] = r;
    }
  }
  jumps++;
  gsym_addr(r,t);
  return r;
}

void gjmp_addr(int a)
{
  gjmp(a);
}

int gtst(int inv, int t)
{
  int v,r;
  v = vtop->r & VT_VALMASK;
  r = ind;
  pr("; gtst inv %d t %d v %d r %d ind %d\n",inv,t,v,r,ind);
  if(v == VT_CMP) {
    pr("; cmp op 0x%x inv %d v %d r %d\n",vtop->c.i,inv,v,r);
    //gsym(t);
    switch(vtop->c.i) {
    case TOK_NE:
      // remember that we need a label to jump to
      jump[jumps][0] = r;
      pr("; cmp ne\n");
      // branches (too short) pr("b%s " LOCAL_LABEL "\n", inv?"eq":"ne", jumps++);
      pr("b%s +\n", inv?"ne":"eq");
      gsym(t);
      pr("brl " LOCAL_LABEL "\n+\n", jumps++);
      break;
    default:
      error("unknown compare");
      break;
    }
    t = r;
  }
  else if(v == VT_JMP || v == VT_JMPI) {
    pr("; VT_jmp r %d t %d ji %d inv %d vtop->c.i %d\n",r,t, v&1, inv, vtop->c.i);
    if((v & 1) == inv) {
      gsym(t);
      t = vtop->c.i;
    }
    else {
      t = gjmp(t);
      gsym(vtop->c.i);
    }
  }
  else {
    if(is_float(vtop->type.t)) {
      pr("; float 4\n");
      v = gv(RC_FLOAT);
      gsym(t);
      pr("lda.b tcc__f%d\nand.w #$ff00\nora.b tcc__f%dh\n", v - TREG_F0, v - TREG_F0);
      vtop->r = VT_CMP;
      vtop->c.i = TOK_NE;
      return gtst(inv, t);
    }
    else if((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
      if((vtop->type.t & VT_BTYPE) == VT_LLONG) error("zweiundvierzig");
      if((vtop->c.i != 0) != inv) {
        pr("; uncond jump: go! (vtop->c.i %d, inv %d)\n",vtop->c.i, inv);
        /* set flags as if we had a false compare result */
        pr("lda.w #0\n");
        t = gjmp(t);
      }
      else pr("; uncond jump: nop\n");
    }
    else {
      v = gv(RC_INT);
      gsym(t);
      pr("; tcc__r%d to compare reg\n", v);
      pr("lda.b tcc__r%d ; DON'T OPTIMIZE\n",v);
      if((vtop->type.t & VT_BTYPE) == VT_LLONG) pr("ora.b tcc__r%d\n", vtop->r2);
      vtop->r = VT_CMP;
      vtop->c.i = TOK_NE;
      return gtst(inv, t);
    }
  }
  vtop--;
  pr("; gtst finished; t %d\n",t);
  return t;
}

// generate an integer operation
void gen_opi(int op)
{
  int r, fr, fc, ft, c, r5;
  char* opcrem = 0, *opcalc = 0, *opcarry = 0;
  int optone;
  int docarry;
  int sign;
  int div;
  int length, align;
  int isconst = 0;
  int timesshift, i;
  
  length = type_size(&vtop[0].type, &align);
  r = vtop[-1].r;
  fr = vtop[0].r;
  fc = vtop[0].c.ul;
  
  // get the actual values
  if((fr & VT_VALMASK) == VT_CONST && op != TOK_UMULL && !(fr & VT_SYM)) {
    // vtop is const, only need to load the other one
    ft = vtop[0].type.t;
    vtop--;
    r = gv(RC_INT);
    isconst = 1;
    if(length <= 2 && (fc < -32768 || fc > 65535)) {
      warning("large integer implicitly truncated");
      fc = (short)fc;
    }
  }
  else {
    // have to load both operands to registers
    gv2(RC_INT,RC_INT);
    r = vtop[-1].r;
    fr = vtop[0].r;
    ft = vtop[0].type.t;
    vtop--;
  }

  pr("; gen_opi len %d op %c\n",length,op);
  switch(op) {
    // multiplication
    case '*':
      if(isconst) {
        pr("; mul #%d, tcc__r%d\n", fc, r);
        pr("lda.w #%d\nsta.b tcc__r9\n", fc);
      }
      else {
        pr("; mul tcc__r%d,tcc__r%d\n",fr,r);
        pr("lda.b tcc__r%d\nsta.b tcc__r9\n", fr);
      }
      pr("lda.b tcc__r%d\nsta.b tcc__r10\n", r);
      pr("jsr.l tcc__mul\n");
      pr("sta.b tcc__r%d\n", r);
      break;

    case TOK_UMULL:
      r = vtop[0].r2 = get_reg(RC_INT);
      c = vtop[0].r;
      vtop[0].r = get_reg(RC_INT);
      pr("; umull tcc__r%d, tcc__r%d => tcc__r%d/tcc__r%d\n", c, vtop[1].r, vtop->r, r);
      pr("lda.b tcc__r%d\nsta.b tcc__r9\nstz.b tcc__r9h\nlda.b tcc__r%d\nsta.b tcc__r10\nstz.b tcc__r10h\n", c, vtop[1].r);
      pr("jsr.l tcc__mull\n");
      pr("stx.b tcc__r%d\nsty.b tcc__r%d\n", r, vtop->r);
      break;
    // division and friends
    case TOK_PDIV:
      op = TOK_UDIV;
    case '/':
    case TOK_UDIV:
    case '%':
    case TOK_UMOD:
      if(op == '/' || op == '%') sign = 1;
      else sign = 0;
      if(op == '/' || op == TOK_UDIV) div = 1;
      else div = 0;
      
      if(isconst) {
        pr("; div #%d, tcc__r%d\n", fc, r);
        pr("ldx.b tcc__r%d\n", r);
        pr("lda.w #%d\n", fc);
      }
      else {
        pr("; div tcc__r%d,tcc__r%d\n", fr, r);

        pr("ldx.b tcc__r%d\n", r); // dividend to x
        pr("lda.b tcc__r%d\n", fr);	// divisor to accu
      }
      pr("jsr.l tcc__%s\n",sign?"div":"udiv");

      if(div) pr("lda.b tcc__r9\nsta.b tcc__r%d\n", r);	// quotient in r9...
      else pr("stx.b tcc__r%d\n", r); // ...remainder in x
      
      break;

    // intops the 65816 can do in hardware      
    case '+':
    case TOK_ADDC1:
    case TOK_ADDC2:
    case '-':
    case TOK_SUBC1:
    case TOK_SUBC2:
    case '&':
    case TOK_LAND:
    case '|':
    case '^':
      optone = 1;
      docarry = 1;
      if(isconst && fc < 0) {
        if(op == '+') { op = '-'; fc = -fc; }
        else if(op == '-') { op = '+'; fc = -fc; }
        else if(op == TOK_ADDC1) { op = TOK_SUBC1; fc = -fc; }
        else if(op == TOK_SUBC1) { op = TOK_ADDC1; fc = -fc; }
        else if(op == TOK_ADDC2) { op = TOK_SUBC2; fc = -fc; }
        else if(op == TOK_SUBC2) { op = TOK_ADDC2; fc = -fc; }
      }
      if(op == '+' || op == TOK_ADDC1 || op == TOK_ADDC2) {
        opcalc = "adc";	// insn to use
        opcrem = "inc";	// *crement to use when doing this op with *crements
        opcarry = "clc"; // how to initialize the carry properly
        if(op == TOK_ADDC2) docarry = 0;
      }
      else if(op == '-' || op == TOK_SUBC1 || op == TOK_SUBC2) {
        opcalc = "sbc";
        opcrem = "dec";
        opcarry = "sec";
        if(op == TOK_SUBC2) docarry = 0;
      }
      else if(op == '&' || op == TOK_LAND) {
        optone = 0;	// optimize the "constant 1" operand case by using a *crement?
        docarry = 0;	// touch the carry?
        opcalc = "and";
      }
      else if(op == '|') {
        optone = 0;
        docarry = 0;
        opcalc = "ora";
      }
      else if(op == '^') {
        optone = 0;
        docarry = 0;
        opcalc = "eor";
      }
      else error("ICE 42");
        
      pr("; %s tcc__r%d (0x%x), tcc__r%d (0x%x) (fr type 0x%x c %d r type 0x%x)\n",opcalc, fr, fr,r,r,vtop[0].type.t,vtop[0].c.ul, vtop[-1].type.t);
      if(isconst) {
        pr("; length xxy %d vtop->type 0x%x\n", type_size(&vtop->type, &align),vtop->type.t);
        if (length == 4) {
          /* probably pointer arithmetic... */
          pr("; assuming pointer arith\n");
          if ((fc >> 16) == 0) pr("stz.b tcc__r%dh\n", r);
          else pr("lda.w #%d\nsta.b tcc__r%dh\n", fc >> 16, r);
        }
        if(fc == 1 && optone) pr("%s.b tcc__r%d\n", opcrem, r);
        else if(fc == 2 && optone) pr("%s.b tcc__r%d\n%s.b tcc__r%d\n", opcrem, r, opcrem, r);
        else pr("%s\nlda.b tcc__r%d\n%s.w #%d\nsta.b tcc__r%d\n", docarry?opcarry:"; nop", r, opcalc, fc & 0xffff, r);
      }
      else {
        pr("; length xxy %d vtop->type 0x%x\n", type_size(&vtop->type, &align),vtop->type.t);
        pr("%s\nlda.b tcc__r%d\n%s.b tcc__r%d\nsta.b tcc__r%d\n", docarry?opcarry:"; nop", r, opcalc, fr, r);
      }
      break;
    
    case TOK_EQ:
    case TOK_NE:
      r5 = get_reg(RC_R5);
      if(isconst) {
        pr("; cmpr(n)eq tcc__r%d, #%d\n", r, fc);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc #%d\n", r, fc);
      }
      else {
        pr("; cmpr(n)eq tcc__r%d, tcc__r%d\n", r, fr);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc.b tcc__r%d\n", r, fr);
      }
      pr("tay\n"); // save for long long comparisons
      if(op == TOK_EQ) pr("beq +\n");
      else pr("bne +\n");
      pr("dex\n+\nstx.b tcc__r%d\n", r5);			// long long code does some wild branching and fucks up
                                                        // if the results of the compares it generates do not
                                                        // end up in the same register; unlikely to be a performance
                                                        // impediment: TCC does not usually use this register anyway
      vtop->r = r5;
      break;
    
    case TOK_GT:
    case TOK_LE:
    case TOK_LT:
    case TOK_GE:
      // 65xxx signed compare logic from here: http://www.6502.org/tutorials/compare_beyond.html#5
      r5 = get_reg(RC_R5);
      if(isconst) {
        pr("; cmpcd tcc__r%d, #%d\n", r, fc);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc.w #%d\n",r,fc);
      }
      else {
        pr("; cmpcd tcc__r%d, tcc__r%d\n", r, fr);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc.b tcc__r%d\n", r, fr);
      }
      pr("tay\n"); // may need that later for long long
      if(op == TOK_GT) pr("beq ++\n"); // greater than => equality not good, result 0
      else if(op == TOK_LE) pr("beq +++\n");	// less than or equal => equality good, result 1
      pr("bvc +\neor #$8000\n+\n");
      switch(op) {
      case TOK_GT: pr("bpl +++\n"); break;
      case TOK_LE: pr("bmi +++\n"); break;
      case TOK_LT: pr("bmi +++\n"); break;
      case TOK_GE: pr("bpl +++\n"); break;
      default: error("don't know how to handle signed 0x%x\n", op);
      }
      pr("++\ndex\n+++\nstx.b tcc__r%d\n", r5);	// see TOK_EQ/TOK_NE
      vtop->r = r5;
      break;
    case TOK_UGT:
    case TOK_ULE:
    case TOK_ULT:
    case TOK_UGE:
      r5 = get_reg(RC_R5);
      if(isconst) {
        pr("; ucmpcd tcc__r%d, #%d\n", r, fc);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc.w #%d\n", r, fc);
      }
      else {
        pr("; ucmpcd tcc__r%d, tcc__r%d\n", r, fr);
        pr("ldx #1\nlda.b tcc__r%d\nsec\nsbc.b tcc__r%d\n", r, fr);
      }
      pr("tay\n"); // needed for long long comparisons
      switch(op) {
      case TOK_UGT: pr("beq +\nbcs ++\n"); break;
      case TOK_ULE: pr("beq ++\nbcc ++\n"); break;
      case TOK_ULT: pr("bcc ++\n"); break;
      case TOK_UGE: pr("bcs ++\n"); break;
      default: error("don't know how to handle 0x%x\n", op);
      }
      pr("+ dex\n++\nstx.b tcc__r%d\n", r5);	// see TOK_EQ/TOK_NE
      vtop->r = r5;
      break;
    case TOK_SAR:
    case TOK_SHR:
    case TOK_SHL:
      timesshift = 1;
#define UNROLL_SHIFT_MAX 4
#define SHIFT_IN_PLACE_MAX 2
      if(isconst) {
        pr("; %s tcc__r%d, #%d\n", op==TOK_SAR?"sar":op==TOK_SHR?"shr":"shl", r, fc);
        if(!fc) return; // 0 -> nothing to do
        if(fc == 8 && (op == TOK_SHR || op == TOK_SHL)) {
          pr("lda.b tcc__r%d\nxba\n", r);
          switch(op) {
            case TOK_SHL: pr("and #$ff00\n"); break;
            case TOK_SHR: pr("and #$00ff\n"); break;
            default: error("ICE 43");
          }
          pr("sta.b tcc__r%d\n", r);
          return;
        }
        else if(fc > UNROLL_SHIFT_MAX)	// too many shifts -> need a loop
          pr("lda.b tcc__r%d\nldy.w #%d\n-\n", r, fc);
        else if(fc > SHIFT_IN_PLACE_MAX) {
          pr("lda.b tcc__r%d\n", r);
          timesshift = fc;
        }
        else {
          // very few shifts; don't bother loading the value to the accu
          for(i = 0; i < fc; i++) {
            switch(op) {
              case TOK_SAR:
                pr("cmp #$8000\n");	// carry <= number negative?
                pr("ror.b tcc__r%d\n", r);
                break;
              case TOK_SHR:
                pr("lsr.b tcc__r%d\n",r);	break;
              case TOK_SHL:
                // for left shifts, signedness is irrelevant (in spite of what the mnemonic seems to suggest)
                pr("asl.b tcc__r%d\n", r); break;
              default: error("unknown shift");
            }
          }
          return;
        }
      }
      else {
        pr("; %s tcc__r%d, tcc__r%d\n", op==TOK_SAR?"sar":op==TOK_SHR?"shr":"shl", r, fr);
        pr("lda.b tcc__r%d\nldy.b tcc__r%d\nbeq +\n-\n", r, fr);
      }
      for(i = 0; i < timesshift; i++)
        switch(op) {
          case TOK_SAR:
            pr("cmp #$8000\n");	// carry <= number negative?
            pr("ror a\n");
            break;
          case TOK_SHR:
            pr("lsr a\n");	break;
          case TOK_SHL:
            // for left shifts, signedness is irrelevant (in spite of what the mnemonic seems to suggest)
            pr("asl a\n"); break;
          default: error("unknown shift");
        }
      if(isconst && fc <= UNROLL_SHIFT_MAX) pr("sta.b tcc__r%d\n", r);
      else pr("dey\nbne -\n+\nsta.b tcc__r%d\n", r);
      break;
    default:
      error("opi 0x%x (%c) unimplemented\n",op,op);
  }
}

void float_to_woz(float, unsigned char*);

void gen_opf(int op)
{
  int r, fr, ft;
  float fcf;
  int length, align;
  int ir;
  
  length = type_size(&vtop[0].type, &align);
  r = vtop[-1].r;
  fr = vtop[0].r;
  fcf = vtop[0].c.f;

  // get the actual values
  gv2(RC_F1,RC_F0);
  r = vtop[-1].r;
  fr = vtop[0].r;
  ft = vtop[0].type.t;
  vtop--;
  
  pr("; gen_opf len %d op 0x%x ('%c')\n",length,op,op);
  
  switch(op) {
    case '*':
      pr("jsr.l tcc__fmul\n");
      break;

    case '/':
      pr("jsr.l tcc__fdiv\n");
      break;

    case '+':
      pr("jsr.l tcc__fadd\n");
      break;
    
    case '-':
      pr("jsr.l tcc__fsub\n");
      break;
      
    case TOK_EQ:
    case TOK_NE:
      ir = get_reg(RC_INT);
      pr("jsr.l tcc__fcmp\n");
      pr("dec a\n");
      if(op == TOK_EQ) pr("beq +\n");
      else pr("bne +\n");
      pr("dex\n+\nstx.b tcc__r%d\n", ir);
      vtop->r = ir;
      return;
      break;
      
    case TOK_GT:
    case TOK_LE:
    case TOK_LT:
    case TOK_GE:
      ir = get_reg(RC_INT);	// register to store the result flag in
      pr("jsr.l tcc__fcmp\n");
      // assuming tcc__fcmp returns signum + 1 in accu and inits X with 1
      // subtracting one to set overflow flag
      pr("sec\nsbc.w #1\n");
      if(op == TOK_GT) pr("beq ++\n"); // greater than => equality not good, result 0
      else if(op == TOK_LE) pr("beq +++\n");	// less than or equal => equality good, result 1
      pr("bvc +\neor #$8000\n");
      switch(op) {
      case TOK_GT: pr("+ bpl +++\n"); break;
      case TOK_LE: pr("+ bmi +++\n"); break;
      case TOK_LT: pr("+ bmi +++\n"); break;
      case TOK_GE: pr("+ bpl +++\n"); break;
      default: error("don't know how to handle signed 0x%x\n", op);
      }
      pr("++\ndex\n+++\nstx.b tcc__r%d\n", ir);
      vtop->r = ir;
      return;
      break;

    default:
      error("opf 0x%x (%c) unimplemented\n",op,op);
  }
  vtop->r = TREG_F0;
}

void gen_cvt_itof(int t)
{
  int r, r2, it;
  gv(RC_INT);		// load integer to convert
  r = vtop->r;		// register with int
  r2 = vtop->r2;	// register with high word (for long longs)
  it = vtop->type.t;	// type of int
  
  pr("; itof tcc__r%d, f0\n", r);
  if((vtop->type.t & VT_BTYPE) == VT_LLONG) {
    pr("pei (tcc__r%d)\npei (tcc__r%d)\n", r2, r);
    if(it & VT_UNSIGNED) error("jsr.l tcc__ulltof\n"); // this is probably dead code
    else pr("jsr.l tcc__lltof\n");
    pr("pla\npla\n");
  }
  else {
    get_reg(RC_F0);		// result will go to f0
    pr("lda.b tcc__r%d\n", r);
    pr("xba\nsta.b tcc__f0 + 1\n");	// convert to big-endian and load to upper 2 bytes of mantissa
    if(it & VT_UNSIGNED) pr("jsr.l tcc__ufloat\n");
    else pr("jsr.l tcc__float\n");
  }
  vtop->r = TREG_F0;	// tell TCC that the result is in f0
}

void gen_cvt_ftoi(int t)
{
  int r = 0;
  gv(RC_F0);
  if((t & VT_BTYPE) == VT_LLONG) {
    get_reg(RC_R0);
    get_reg(RC_R1);
  }
  else r = get_reg(RC_INT);
  pr("; ftoi tcc__f0, tcc__r%d(type 0x%x)\n", r, t);
  if(t & VT_UNSIGNED) pr("lda #0\nsta.b tcc__r9\n");
  else pr("lda #1\nsta.b tcc__r9\n");
  
  // use llfix for any unsigned type to avoid overflow
  if((t & VT_BTYPE) == VT_LLONG || (t & VT_UNSIGNED)) {
    pr("jsr.l tcc__llfix\n");
    vtop->r2 = TREG_R1;
    vtop->r = TREG_R0;
    return;
  }
  else {
    pr("jsr.l tcc__fix\n");
    pr("lda.b tcc__f0 + 1\nxba\nsta.b tcc__r%d\n", r);
  }
  
  vtop->r = r;
}

void gen_cvt_ftof(int t)
{
  error("gen_cvt_ftof 0x%x\n",t);
}

void ggoto(void)
{
  int r = gv(RC_INT);
  int t = vtop->type.t;
  pr("; ggoto r 0x%x t 0x%x\n",r,t);
  pr("lda.b tcc__r%d\nsta.b tcc__r9 + 1\nsep #$20\nlda.b tcc__r%dh\nsta.b tcc__r9h + 1\nlda.b #$5c\nsta.b tcc__r9\nrep #$20\n", r, r);
  pr("jml.l tcc__r9\n");
}

int section_count = 0;
int ind_before_section = 0;
int section_closed = 1;

void gfunc_prolog(CType* func_type)
{
  Sym* sym; //, *sym2;
  Sym* symf;
  int n,addr,size,align;
  //fprintf(stderr,"gfunc_prolog t %d sym %p\n",func_type->t,func_type->ref);

  sym = func_type->ref;
  func_vt = sym->type;
  
  n=0;
  addr=3;	// skip 24-bit return address
  loc = 0;
  if((func_vt.t & VT_BTYPE) == VT_STRUCT) {
    func_vc = addr;
    addr += PTR_SIZE;
    n += PTR_SIZE;
  }

  /* super-dirty hack to get the function name */
  symf = (Sym*) ( ((void*)func_type) - offsetof(Sym, type) );
  strcpy(current_fn, get_sym_str(symf));

  /* wlalink does not cut up sections, so it is desirable to have a section
     for each function to keep the amount of unused memory in the ROM banks
     low. WLA DX barfs, however, if fed more than 255 sections, so we have
     to be a bit economical: gfunc_epilog() only closes the section if more
     than 50K of assembler code have been written */
  if(section_closed) {
    ind_before_section = ind;
    pr("\n.section \".text_0x%x\" superfree\n", section_count++);
    section_closed = 0;
  }
    
  pr("\n%s:\n",current_fn);

  while((sym = sym->next)) {
    CType* type;
    type = &sym->type;
    sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | VT_LVAL, addr);
    size = type_size(type, &align);
    //fprintf(stderr,"pushed sym type 0x%x size %d addr 0x%x\n",type->t,size,addr);
    addr += size;
    n += size;
  }
  pr("; sub sp,#__%s_locals\n",current_fn);
  pr(".ifgr __%s_locals 0\ntsa\nsec\nsbc #__%s_locals\ntas\n.endif\n",current_fn,current_fn);
  loc = 0; // huh squared?
}

char locals[1000][80];
int localnos[1000];
int localno=0;

void gfunc_epilog(void)
{
  pr("; add sp, #__%s_locals\n",current_fn);
  pr(".ifgr __%s_locals 0\ntsa\nclc\nadc #__%s_locals\ntas\n.endif\n", current_fn, current_fn);
  pr("rtl\n");
  
  pr(".ends\n");
  section_closed = 1;
  
  if(-loc > 0x1f00) error("stack overflow");
  /* simply putting a ".define __<current_fn>_locals -<loc>" after the
     function does not work in some cases for unknown reasons (wla-dx
     complains about unresolved symbols); putting them before the reference
     works, but this has to be done by the output code, so we have to save
     the various locals sizes somewhere */
  strcpy(locals[localno], current_fn);
  localnos[localno] = -loc;
  localno++;
  current_fn[0] = 0;
}
