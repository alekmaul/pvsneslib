/*
 *  ELF file handling for TCC
 * 
 *  Copyright (c) 2001-2004 Fabrice Bellard
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

static int put_elf_str(Section *s, const char *sym)
{
    int offset, len;
    char *ptr;

    //fprintf(stderr,"put_elf_str %s\n",sym);
    len = strlen(sym) + 1;
    offset = s->data_offset;
    ptr = section_ptr_add(s, len);
    memcpy(ptr, sym, len);
    return offset;
}

/* elf symbol hashing function */
static unsigned long elf_hash(const unsigned char *name)
{
    unsigned long h = 0, g;
    
    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h;
}

/* rebuild hash table of section s */
/* NOTE: we do factorize the hash table code to go faster */
static void rebuild_hash(Section *s, unsigned int nb_buckets)
{
    Elf32_Sym *sym;
    int *ptr, *hash, nb_syms, sym_index, h;
    char *strtab;

    strtab = s->link->data;
    nb_syms = s->data_offset / sizeof(Elf32_Sym);

    s->hash->data_offset = 0;
    ptr = section_ptr_add(s->hash, (2 + nb_buckets + nb_syms) * sizeof(int));
    ptr[0] = nb_buckets;
    ptr[1] = nb_syms;
    ptr += 2;
    hash = ptr;
    memset(hash, 0, (nb_buckets + 1) * sizeof(int));
    ptr += nb_buckets + 1;

    sym = (Elf32_Sym *)s->data + 1;
    for(sym_index = 1; sym_index < nb_syms; sym_index++) {
        if (ELF32_ST_BIND(sym->st_info) != STB_LOCAL) {
            h = elf_hash(strtab + sym->st_name) % nb_buckets;
            *ptr = hash[h];
            hash[h] = sym_index;
        } else {
            *ptr = 0;
        }
        ptr++;
        sym++;
    }
}

/* return the symbol number */
static int put_elf_sym(Section *s, 
                       unsigned long value, unsigned long size,
                       int info, int other, int shndx, const char *name)
{
    int name_offset, sym_index;
    int nbuckets, h;
    Elf32_Sym *sym;
    Section *hs;
    //fprintf(stderr,"\\\\\\ putting elf symbol %s in section type %d, section %p, value %ld size %ld info %d other %d\n",name,s->sh_type,s,value,size,info,other);
    // HACK
    //info |= 0x10;
    sym = section_ptr_add(s, sizeof(Elf32_Sym));
    if (name)
        name_offset = put_elf_str(s->link, name);
    else
        name_offset = 0;
    /* XXX: endianness */
    sym->st_name = name_offset;
    sym->st_value = value;
    sym->st_size = size;
    sym->st_info = info;
    sym->st_other = other;
    sym->st_shndx = shndx;
    sym_index = sym - (Elf32_Sym *)s->data;
    hs = s->hash;
    if (hs) {
        int *ptr, *base;
        ptr = section_ptr_add(hs, sizeof(int));
        base = (int *)hs->data;
        /* only add global or weak symbols */
        if (ELF32_ST_BIND(info) != STB_LOCAL) {
            /* add another hashing entry */
            nbuckets = base[0];
            h = elf_hash(name) % nbuckets;
            *ptr = base[2 + h];
            base[2 + h] = sym_index;
            base[1]++;
            /* we resize the hash table */
            hs->nb_hashed_syms++;
            if (hs->nb_hashed_syms > 2 * nbuckets) {
                rebuild_hash(s, 2 * nbuckets);
            }
        } else {
            *ptr = 0;
            base[1]++;
        }
    }
    return sym_index;
}

/* find global ELF symbol 'name' and return its index. Return 0 if not
   found. */
static int find_elf_sym(Section *s, const char *name)
{
    Elf32_Sym *sym;
    Section *hs;
    int nbuckets, sym_index, h;
    const char *name1;
    
    hs = s->hash;
    if (!hs)
        return 0;
    nbuckets = ((int *)hs->data)[0];
    h = elf_hash(name) % nbuckets;
    sym_index = ((int *)hs->data)[2 + h];
    while (sym_index != 0) {
        sym = &((Elf32_Sym *)s->data)[sym_index];
        name1 = s->link->data + sym->st_name;
        if (!strcmp(name, name1))
            return sym_index;
        sym_index = ((int *)hs->data)[2 + nbuckets + sym_index];
    }
    return 0;
}

/* return elf symbol value or error */
int tcc_get_symbol(TCCState *s, unsigned long *pval, const char *name)
{
    int sym_index;
    Elf32_Sym *sym;
    
    sym_index = find_elf_sym(symtab_section, name);
    if (!sym_index)
        return -1;
    sym = &((Elf32_Sym *)symtab_section->data)[sym_index];
    *pval = sym->st_value;
    return 0;
}

/* return elf symbol value or error */
Elf32_Sym* tcc_really_get_symbol(TCCState *s, unsigned long *pval, const char *name)
{
    int sym_index;
    Elf32_Sym *sym;
    
    sym_index = find_elf_sym(symtab_section, name);
    if (!sym_index)
        return NULL;
    sym = &((Elf32_Sym *)symtab_section->data)[sym_index];
    *pval = sym->st_value;
    return sym;
}

void *tcc_get_symbol_err(TCCState *s, const char *name)
{
    unsigned long val;
    if (tcc_get_symbol(s, &val, name) < 0)
        error("%s not defined", name);
    return (void *)val;
}

/* add an elf symbol : check if it is already defined and patch
   it. Return symbol index. NOTE that sh_num can be SHN_UNDEF. */
static int add_elf_sym(Section *s, unsigned long value, unsigned long size,
                       int info, int other, int sh_num, const char *name)
{
    Elf32_Sym *esym;
    int sym_bind, sym_index, sym_type, esym_bind;

    sym_bind = ELF32_ST_BIND(info);
    sym_type = ELF32_ST_TYPE(info);
        
    if (sym_bind != STB_LOCAL) {
        /* we search global or weak symbols */
        sym_index = find_elf_sym(s, name);
        if (!sym_index)
            goto do_def;
        esym = &((Elf32_Sym *)s->data)[sym_index];
        if (esym->st_shndx != SHN_UNDEF) {
            esym_bind = ELF32_ST_BIND(esym->st_info);
            if (sh_num == SHN_UNDEF) {
                /* ignore adding of undefined symbol if the
                   corresponding symbol is already defined */
            } else if (sym_bind == STB_GLOBAL && esym_bind == STB_WEAK) {
                /* global overrides weak, so patch */
                goto do_patch;
            } else if (sym_bind == STB_WEAK && esym_bind == STB_GLOBAL) {
                /* weak is ignored if already global */
            } else {
#if 0
                printf("new_bind=%d new_shndx=%d last_bind=%d old_shndx=%d\n",
                       sym_bind, sh_num, esym_bind, esym->st_shndx);
#endif
                /* NOTE: we accept that two DLL define the same symbol */
                if (s != tcc_state->dynsymtab_section)
                    error_noabort("'%s' defined twice", name);
            }
        } else {
        do_patch:
            esym->st_info = ELF32_ST_INFO(sym_bind, sym_type);
            esym->st_shndx = sh_num;
            esym->st_value = value;
            esym->st_size = size;
            esym->st_other = other;
        }
    } else {
    do_def:
        sym_index = put_elf_sym(s, value, size, 
                                ELF32_ST_INFO(sym_bind, sym_type), other, 
                                sh_num, name);
    }
    return sym_index;
}

/* put relocation */
static void put_elf_reloc(Section *symtab, Section *s, unsigned long offset,
                          int type, int symbol)
{
    char buf[256];
    Section *sr;
    Elf32_Rel *rel;

    sr = s->reloc;
    if (!sr) {
        /* if no relocation section, create it */
        snprintf(buf, sizeof(buf), ".rel%s", s->name);
        /* if the symtab is allocated, then we consider the relocation
           are also */
        sr = new_section(tcc_state, buf, SHT_REL, symtab->sh_flags);
        sr->sh_entsize = sizeof(Elf32_Rel);
        sr->link = symtab;
        sr->sh_info = s->sh_num;
        s->reloc = sr;
    }
    rel = section_ptr_add(sr, sizeof(Elf32_Rel));
    rel->r_offset = offset;
    rel->r_info = ELF32_R_INFO(symbol, type);
}

/* put stab debug information */

typedef struct {
    unsigned long n_strx;         /* index into string table of name */
    unsigned char n_type;         /* type of symbol */
    unsigned char n_other;        /* misc info (usually empty) */
    unsigned short n_desc;        /* description field */
    unsigned long n_value;        /* value of symbol */
} Stab_Sym;

static void put_stabs(const char *str, int type, int other, int desc, 
                      unsigned long value)
{
    Stab_Sym *sym;

    sym = section_ptr_add(stab_section, sizeof(Stab_Sym));
    if (str) {
        sym->n_strx = put_elf_str(stabstr_section, str);
    } else {
        sym->n_strx = 0;
    }
    sym->n_type = type;
    sym->n_other = other;
    sym->n_desc = desc;
    sym->n_value = value;
}

static void put_stabs_r(const char *str, int type, int other, int desc, 
                        unsigned long value, Section *sec, int sym_index)
{
    put_stabs(str, type, other, desc, value);
    put_elf_reloc(symtab_section, stab_section, 
                  stab_section->data_offset - sizeof(unsigned long),
                  R_DATA_32, sym_index);
}

static void put_stabn(int type, int other, int desc, int value)
{
    put_stabs(NULL, type, other, desc, value);
}

static void put_stabd(int type, int other, int desc)
{
    put_stabs(NULL, type, other, desc, 0);
}

char** relocptrs = NULL;

/* relocate a given section (CPU dependent) */
static void relocate_section(TCCState *s1, Section *s)
{
    Section *sr;
    Elf32_Rel *rel, *rel_end, *qrel;
    Elf32_Sym *sym;
    int type, sym_index;
    unsigned char *ptr;
    unsigned long val, addr;
#if defined(TCC_TARGET_I386)
    int esym_index;
#endif

    if (!relocptrs) {
        relocptrs = calloc(0x100000, sizeof(char *));
    }
    
    sr = s->reloc;
    rel_end = (Elf32_Rel *)(sr->data + sr->data_offset);
    qrel = (Elf32_Rel *)sr->data;
    for(rel = qrel;
        rel < rel_end;
        rel++) {
        ptr = s->data + rel->r_offset;

        sym_index = ELF32_R_SYM(rel->r_info);
        sym = &((Elf32_Sym *)symtab_section->data)[sym_index];
        val = sym->st_value;
        type = ELF32_R_TYPE(rel->r_info);
        addr = s->sh_addr + rel->r_offset;

        /* CPU specific */
        switch(type) {
#if defined(TCC_TARGET_816)
        case R_DATA_32:
            //fprintf(stderr,"___ relocating at 0x%lx to 0x%lx, sr %p, s %p, shndx %d name %s info 0x%x other 0x%x relocindex 0x%x ptr 0x%x\n",addr,val,sr,s,sym->st_shndx,symtab_section->link->data + sym->st_name, sym->st_info, sym->st_other,relocindices[addr],*(unsigned int*)ptr);
            if(relocptrs[((unsigned long)ptr)&0xfffff]) error("relocptrs collision");
            /* if(ELF32_ST_BIND(sym->st_info) == STB_LOCAL) {
              relocptrs[((unsigned int)ptr)&0xfffff] = calloc(1,strlen(static_prefix) + strlen(symtab_section->link->data + sym->st_name) + 1);
              sprintf(relocptrs[((unsigned int)ptr)&0xfffff], "%s%s", static_prefix, symtab_section->link->data + sym->st_name);
            }
            else */ relocptrs[((unsigned long)ptr)&0xfffff] = symtab_section->link->data + sym->st_name;
            /* no need to change the value at ptr, we only need the offset, and that's already there */
            break;
        default:
            fprintf(stderr,"FIXME: handle reloc type 0x%x at 0x%lx [%.8lx] to 0x%lx\n",
                    type, addr, (unsigned long)ptr, val);
            break;
#else
#error unsupported processor
#endif
        }
    }
    /* if the relocation is allocated, we change its symbol table */
    if (sr->sh_flags & SHF_ALLOC)
        sr->link = s1->dynsym;
}

static Section *new_symtab(TCCState *s1,
                           const char *symtab_name, int sh_type, int sh_flags,
                           const char *strtab_name, 
                           const char *hash_name, int hash_sh_flags)
{
    Section *symtab, *strtab, *hash;
    int *ptr, nb_buckets;

    symtab = new_section(s1, symtab_name, sh_type, sh_flags);
    symtab->sh_entsize = sizeof(Elf32_Sym);
    strtab = new_section(s1, strtab_name, SHT_STRTAB, sh_flags);
    put_elf_str(strtab, "");
    symtab->link = strtab;
    put_elf_sym(symtab, 0, 0, 0, 0, 0, NULL);
    
    nb_buckets = 1;

    hash = new_section(s1, hash_name, SHT_HASH, hash_sh_flags);
    hash->sh_entsize = sizeof(int);
    symtab->hash = hash;
    hash->link = symtab;

    ptr = section_ptr_add(hash, (2 + nb_buckets + 1) * sizeof(int));
    ptr[0] = nb_buckets;
    ptr[1] = 1;
    memset(ptr + 2, 0, (nb_buckets + 1) * sizeof(int));
    return symtab;
}

static void tcc_output_binary(TCCState *s1, FILE *f,
                              const int *section_order)
{
    Section *s;
    int i,j, k, size;

    //fprintf(stderr,"outputting binary, %d sections\n",s1->nb_sections);

#if 0
    Elf32_Sym* esym;
    for(j = 0, esym = (Elf32_Sym*) symtab_section->data; j < symtab_section->sh_size / 4; esym++, j++) {
      fprintf(stderr,"symbol st_shndx %d\n", esym->st_shndx);
      if(esym->st_shndx == 2) fprintf(stderr,"symbol %s value %d\n", symtab_section->link->data + esym->st_name,esym->st_value);
    }
#endif

    /* include header */
    /* fprintf(f, ".incdir \"" CONFIG_TCCDIR "/include\"\n"); */
    fprintf(f, ".include \"hdr.asm\"\n");
    fprintf(f, ".accu 16\n.index 16\n");
    fprintf(f, ".16bit\n");

    /* local variable size constants; used to be generated as part of the
       function epilog, but WLA DX barfed once in a while about missing
       symbols. putting them at the start of the file works around that. */
    for(i=0; i<localno; i++) {
      fprintf(f, ".define __%s_locals %d\n", locals[i], localnos[i]);
    }
    
    /* relocate sections
       this not only rewrites the pointers inside sections (with bogus
       data), but, more importantly, saves the names of the symbols we have
       to output later in place of this bogus data in the relocptrs[] array. */
    for(i=1;i<s1->nb_sections;i++) {
        s = s1->sections[section_order[i]];
        //fprintf(stderr,"___ relocating section %p (%s) (reloc %p)\n",s,s->name,s->reloc);
        if (s->reloc && s != s1->got)
                        relocate_section(s1, s);
    }
    
    /* output sections */
    for(i=1;i<s1->nb_sections;i++) {
        s = s1->sections[section_order[i]];
        //fprintf(stderr,"section %p, sh_size %ld, sh_num %d\n",s,s->sh_size,s->sh_num);

        /* these sections are meaningless when writing plain-text assembler output */        
        if(strcmp(".symtab", s->name) == 0 ||
           strcmp(".strtab", s->name) == 0 ||
           strcmp(".rel.data", s->name) == 0 ||
           strcmp(".shstrtab", s->name) == 0) continue;
        
        //fprintf(f,"\n;.section %p\n",s->reloc);

        size = s->sh_size;	/* section size in bytes */
        //fprintf(stderr,"%d bytes: %s\n",size,s->name);

        if(s == text_section) {
          /* functions each have their own section (otherwise WLA DX is
             not able to allocate ROM space for them efficiently), so we
             do not have to print a function header here */
          int next_jump_pos = 0;	/* the next offset in the text section where we will look for a jump target */
          for(j = 0; j < size; j++) {
            //Elf32_Sym* esym;
            for(k = 0; k < labels; k++) {
              //fprintf(stderr,"label %s at %d\n", label[k].name, label[k].pos);
              if(label[k].pos == j) fprintf(f, "%s%s:\n", static_prefix /* "__local_" */, label[k].name);
            }
            /* insert jump labels */
            if(next_jump_pos == j) {
              next_jump_pos = size;
              for(k = 0; k < jumps; k++) {
                /* while we're here, look for the next jump target after this one */
                if(jump[k][1] > j && jump[k][1] < next_jump_pos) next_jump_pos = jump[k][1];
                /* write the jump target label(s) for this position */
                //if(jump[k][1] == j) fprintf(f, LOCAL_LABEL ": ; at %d\n", k, j);
                if(jump[k][1] == j) fprintf(f, LOCAL_LABEL ":\n", k);
              }
            }
            fputc(s->data[j], f);
          }
          if(!section_closed) fprintf(f, ".ends\n");
        }
        else if(s == bss_section) {
          /* uninitialized data, we only need a .ramsection */
          Elf32_Sym* esym;
          int empty = 1;
          fprintf(f, ".ramsection \".bss\" bank $7e slot 2\n");
          //fprintf(f, "ramsection.bss dsb 0\n");
          for(j = 0, esym = (Elf32_Sym*) symtab_section->data; j < symtab_section->sh_size / sizeof(Elf32_Sym); esym++, j++) {
            //fprintf(stderr,"%d/%d symbol %p st_shndx %d\n", j, symtab_section->sh_size / sizeof(Elf32_Sym*), esym, esym->st_shndx);
            //fprintf(stderr,"esym %p\n", esym);
            //if(esym->st_shndx < 3) fprintf(stderr,"symbol %s\n", symtab_section->link->data + esym->st_name);
              if(esym->st_shndx == SHN_COMMON
                 && strlen(symtab_section->link->data + esym->st_name)) /* omit nameless symbols (fixes 20041218-1.c) */
              {
                /* looks like these are the symbols that need to go here,
                   but that is merely an educated guess. works for me, though. */
                //fprintf(stderr,"COM symbol %s: value %d size %d\n",get_tok_str(ps->v, NULL),esym->st_value,esym->st_size);
                fprintf(f, "%s%s dsb %d\n", /*ELF32_ST_BIND(esym->st_info) == STB_LOCAL ? static_prefix:*/"", symtab_section->link->data + esym->st_name, esym->st_size);
                empty = 0;
              }
          }
#if 0
          for(;ps;ps=ps->prev) {
            if((esym = tcc_really_get_symbol(s1, &pval, get_tok_str(ps->v, NULL)))) {
              if(esym->st_shndx == SHN_COMMON) {
                /* looks like these are the symbols that need to go here,
                   but that is merely an educated guess. works for me, though. */
                //fprintf(stderr,"COM symbol %s: value %d size %d\n",get_tok_str(ps->v, NULL),esym->st_value,esym->st_size);
                fprintf(f, "%s dsb %d\n", get_tok_str(ps->v, NULL), esym->st_size);
                empty = 0;
              }
            }
          }
#endif
          if(empty) fprintf(f, "__local_dummybss dsb 1\n");
          //fprintf(f, "endsection.bss dsb 0\n");
          fprintf(f, ".ends\n");
        }
        else {	/* .data, .rodata, user-defined sections */

          int deebeed = 0;	/* remembers whether we have printed ".db"
                                   before; reset after a newline or a
                                   different sized prefix, e.g. ".dw" */
          int startk = 0;	/* 0 == .ramsection, 1 == .section */
          int endk = 2;		/* do both by default */

          if(s != data_section) startk = 1; /* only do .section (.rodata and user sections go to ROM) */
          
          int bytecount = 0;	/* how many bytes to reserve in .ramsection */

          /* k == 0: output .ramsection; k == 1: output .section */
          for(k = startk; k < endk; k++) {

            if(k == 0) {	/* .ramsection */
              fprintf(f, ".ramsection \"ram%s\" bank $7f slot 3\n",s->name);
              //fprintf(f, "ramsection%s dsb 0\n", s->name);
            }
            else {	/* (ROM) .section */
              fprintf(f, ".section \"%s\" superfree\n", s->name);
              //fprintf(f, "startsection%s:", s->name);
            }

            //int next_symbol_pos = 0;	/* position inside the section at which to look for the next symbol */
            
            for(j=0; j<size; j++) {
              //Sym* ps = global_stack;
              int ps;

              /* check if there is a symbol at this position */
              Elf32_Sym* esym;	/* ELF symbol */
              char* lastsym = NULL;	/* name of previous symbol (some symbols appear more than once; bug?) */
              int symbol_printed = 0; /* have we already printed a symbol in this run? */
              for(ps = 0, esym = (Elf32_Sym*) symtab_section->data; ps < symtab_section->sh_size / sizeof(Elf32_Sym); esym++, ps++) {
                //if(!find_elf_sym(symtab_section, get_tok_str(ps->v, NULL))) continue;
                unsigned long pval;
                char* symname = symtab_section->link->data + esym->st_name;
                char* symprefix = "";
                
                //fprintf(stderr,"gsym %p name %s type 0x%x num %d reg 0x%x\n",ps,get_tok_str(ps->v,NULL),ps->type.t,ps->c,ps->r);

                /* we do not have to care about external references (handled by the linker) or
                   functions (handled by the code generator */
                //if((ps->type.t & VT_EXTERN) || ((ps->type.t & VT_BTYPE) == VT_FUNC)) continue;

                /* look up this symbol */
                pval = esym->st_value;

                //fprintf(stderr,"/// pval %d, j %d esym->st_shndx %d s->sh_num %d\n",pval,j,esym->st_shndx,s->sh_num);
                
                /* look for the next symbol after this one */
                //if(pval > j && pval < next_symbol_pos) next_symbol_pos = pval;

                /* Is this symbol at this position and in this section? */
                if(pval != j || esym->st_shndx != s->sh_num) continue;

                /* skip empty symbols (bug?) */
                if(strlen(symname) == 0) continue;
                /* some symbols appear more than once; avoid defining them more than once (bug?) */
                if(lastsym && !strcmp(lastsym, symname)) continue;
                /* remember this symbol for the next iteration */
                lastsym = symname;
                
                /* if this is a local (static) symbol, prefix it so the assembler knows this
                   is file-local. */
                /* FIXME: breaks for local static variables (name collision) */
                //if(ELF32_ST_BIND(esym->st_info) == STB_LOCAL) { symprefix = static_prefix; }
                  
                /* if this is a ramsection, we now know how large the _previous_ symbol was; print it. */
                /* if we already printed a symbol in this section, define this symbol as size 0 so it
                   gets the same address as the other ones at this position. */
                if(k==0 && (bytecount > 0 || symbol_printed)) {
                  fprintf(f, "dsb %d", bytecount);
                  bytecount = 0;
                }

                /* if there are two sections, print label only in .ramsection */
                if(k == 0) fprintf(f, "\n%s%s ", symprefix, symname);
                else if(startk == 1) fprintf(f,"\n%s%s: ", symprefix, symname);
                else fprintf(f, "\n");
                symbol_printed = 1;
                
              }

              if(symbol_printed) {
                  /* pointers and arrays may have a symbolic name. find out if that's the case.
                     everything else is literal and handled later */
                  //if((ps->type.t & (VT_PTR|VT_ARRAY)) /* == VT_PTR */) {
                  unsigned int ptr = *((unsigned int*)&s->data[j]);
                  unsigned char ptrc = *((unsigned char*)&s->data[j]);
                  //fprintf(stderr, "%%%%%%%%pointer type 0x%x v 0x%x r 0x%x c 0x%x, value 0x%x\n",ps->type.t,ps->v,ps->r,ps->c,ptr);
                  
                  if(k == 0) {	/* .ramsection, just count bytes */
                    bytecount ++;
                  }
                  else {		/* (ROM) .section, need to output data */
                    if(relocptrs && relocptrs[((unsigned long)&s->data[j])&0xfffff]) {
                      /* relocated -> print a symbolic pointer */
                      //fprintf(f,".dw ramsection%s + $%x", s->name, ptr);
                      char* ptrname = relocptrs[((unsigned long)&s->data[j])&0xfffff];
                      fprintf(f,".dw %s + %d, :%s", ptrname, ptr, ptrname);
                      j+=3;	/* we have handled 3 more bytes than expected */
                      deebeed = 0;
                    }
                    else {
                      /* any non-symbolic data; print one byte, then let the generic code take over */
                      //fprintf(f,"; type 0x%x\n", ps->type.t);
                      //if(ps->type.t & VT_BYTE) exit(42);
                      fprintf(f,".db $%x", ptrc);
                      deebeed = 1;
                    }
                  }
                  continue; /* data has been printed, go ahead */
              }
              
              /* no symbol here, just print the data */
              if(k == 1 && relocptrs && relocptrs[((unsigned long)&s->data[j])&0xfffff]) {
                /* unlabeled data may have been relocated, too */
                fprintf(f,"\n.dw %s + %d\n.dw :%s", relocptrs[((unsigned long)&s->data[j])&0xfffff], *(unsigned int*)(&s->data[j]), relocptrs[((unsigned long)&s->data[j])&0xfffff]);
                j+=3;
                deebeed = 0;
                continue;
              }
              
              if(!deebeed) {
                if(k == 1) fprintf(f, "\n.db ");
                deebeed = 1;
              }
              else if(k==1) fprintf(f,",");
              if(k==1) fprintf(f, "$%x",s->data[j]);
              bytecount++;
            }
            if(k==0) { if(!bytecount) { fprintf(f, "__local_dummy%s ", s->name); bytecount++; } fprintf(f, "dsb %d\n", bytecount); bytecount = 0; }
            //if(k==1 && deebeed) fprintf(f,"\n");
            if(k==1) {
              //fprintf(f,"\nendsection%s:", s->name);
              if(!size) fprintf(f, "\n__local_dummy%s: .db 0", s->name);
            }
            fprintf(f,"\n.ends\n\n");
          }
        }

#if 0
        fprintf(stderr,"sh_type %d\n",s->sh_type);
        fprintf(stderr,"index of L.0 %d\n",find_elf_sym(s, "L.0"));

        if(s->sh_type == SHT_REL) {
          for(j = 0; j < s->sh_size; j++) {
            fprintf(stderr,"%02x ", s->data[j]);
            if(j%4 == 3) fprintf(stderr,"\n");
          }
          fprintf(stderr,"\n");
          //fprintf(stderr,"symtab %s\n",s->data);
        }
#endif

    }
    //fprintf(stderr,"index of L.0 in symtab_section %d\n",find_elf_sym(symtab_section,"L.0"));
}

/* output an ELF file */
/* XXX: suppress unneeded sections */
int tcc_output_file(TCCState *s1, const char *filename)
{
    Elf32_Ehdr ehdr;
    FILE *f;
    int fd, mode, ret;
    int *section_order;
    int shnum, i, phnum, file_offset, sh_order_index;
    Section *strsec, *s;
    Elf32_Phdr *phdr;
    Section *interp, *dynamic, *dynstr;
    unsigned long saved_dynamic_data_offset;
    int file_type;
    
    file_type = s1->output_type;
    s1->nb_errors = 0;

    if (file_type != TCC_OUTPUT_OBJ) {
        abort();
    }

    phdr = NULL;
    section_order = NULL;
    interp = NULL;
    dynamic = NULL;
    dynstr = NULL; /* avoid warning */
    saved_dynamic_data_offset = 0; /* avoid warning */
    
    if (file_type != TCC_OUTPUT_OBJ) {
        abort();
    }

    memset(&ehdr, 0, sizeof(ehdr));

    /* we add a section for symbols */
    strsec = new_section(s1, ".shstrtab", SHT_STRTAB, 0);
    put_elf_str(strsec, "");
    
    /* compute number of sections */
    shnum = s1->nb_sections;

    /* this array is used to reorder sections in the output file */
    section_order = tcc_malloc(sizeof(int) * shnum);
    section_order[0] = 0;
    sh_order_index = 1;
    
    /* compute number of program headers */
    switch(file_type) {
    default:
    case TCC_OUTPUT_OBJ:
        phnum = 0;
        break;
    case TCC_OUTPUT_EXE:
        if (!s1->static_link)
            phnum = 4;
        else
            phnum = 2;
        break;
    case TCC_OUTPUT_DLL:
        phnum = 3;
        break;
    }

    /* allocate strings for section names and decide if an unallocated
       section should be output */
    /* NOTE: the strsec section comes last, so its size is also
       correct ! */
    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        s->sh_name = put_elf_str(strsec, s->name);
        /* when generating a DLL, we include relocations but we may
           patch them */
        if (file_type == TCC_OUTPUT_DLL && 
            s->sh_type == SHT_REL && 
            !(s->sh_flags & SHF_ALLOC)) {
            abort();
        } else if (do_debug || 
            file_type == TCC_OUTPUT_OBJ || 
            (s->sh_flags & SHF_ALLOC) ||
            i == (s1->nb_sections - 1)) {
            /* we output all sections if debug or object file */
            s->sh_size = s->data_offset;
        }
    }

    /* allocate program segment headers */
    phdr = tcc_mallocz(phnum * sizeof(Elf32_Phdr));
        
    if (s1->output_format == TCC_OUTPUT_FORMAT_ELF) {
        file_offset = sizeof(Elf32_Ehdr) + phnum * sizeof(Elf32_Phdr);
    } else {
        file_offset = 0;
    }
    if (phnum > 0) {
        abort();
    }

    /* all other sections come after */
    for(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        if (phnum > 0 && (s->sh_flags & SHF_ALLOC))
            continue;
        section_order[sh_order_index++] = i;
        
        file_offset = (file_offset + s->sh_addralign - 1) & 
            ~(s->sh_addralign - 1);
        s->sh_offset = file_offset;
        if (s->sh_type != SHT_NOBITS)
            file_offset += s->sh_size;
    }
    
    /* write elf file */
    if (file_type == TCC_OUTPUT_OBJ)
        mode = 0666;
    else
        mode = 0777;
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode); 
    if (fd < 0) {
        error_noabort("could not write '%s'", filename);
        ret = -1;
        goto the_end;
    }
    f = fdopen(fd, "wb");

    if (s1->output_format == TCC_OUTPUT_FORMAT_ELF) {
        abort();
    } else {
        tcc_output_binary(s1, f, section_order);
    }
    fclose(f);

    ret = 0;
 the_end:
    tcc_free(s1->symtab_to_dynsym);
    tcc_free(section_order);
    tcc_free(phdr);
    tcc_free(s1->got_offsets);
    return ret;
}

