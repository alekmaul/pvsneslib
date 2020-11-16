/* simple malloc()/free() implementation
   adapted from here: http://www.flipcode.com/archives/Simple_Malloc_Free_Functions.shtml */

#define USED 1

typedef struct {
  unsigned int size;
} unit;

typedef struct {
  unit* free;
  unit* heap;
} msys_t;

static msys_t msys;

static unit* __compact(unit *p, unsigned int nsize)
{
       unsigned int bsize, psize;
       unit *best;

       best = p;
       bsize = 0;

       while (psize = p->size, psize) {
              if (psize & USED) {
                  if (bsize != 0) {
                      best->size = bsize;
                      if(bsize >= nsize) {
                          return best;
                      }
                  }
                  bsize = 0;
                  best = p = (unit *)((void*)p + (psize & ~USED));
              }
              else {
                  bsize += psize;
                  p = (unit *)((void*)p + psize);
              }
       }

       if(bsize != 0) {
           best->size = bsize;
           if(bsize >= nsize) {
               return best;
           }
       }

       return 0;
}

void free(void *ptr)
{
     if(ptr) {
         unit *p;

         p = (unit *)((void*)ptr - sizeof(unit));
         p->size &= ~USED;
     }
}

void *malloc(unsigned int size)
{
     unsigned int fsize;
     unit *p;

     if(size == 0) return 0;

     size  += 3 + sizeof(unit);
     size >>= 2;
     size <<= 2;

     if(msys.free == 0 || size > msys.free->size) {
         msys.free = __compact(msys.heap, size);
         if(msys.free == 0) return 0;
     }

     p = msys.free;
     fsize = msys.free->size;

     if(fsize >= size + sizeof(unit)) {
         msys.free = (unit *)((void*)p + size);
         msys.free->size = fsize - size;
     }
     else {
         msys.free = 0;
         size = fsize;
     }

     p->size = size | USED;

     return (void *)((void*)p + sizeof(unit));
}

void malloc_init(void *heap, unsigned int len)
{
     len >>= 2;
     len <<= 2;
     msys.free = msys.heap = (unit *) heap;
     msys.free->size = msys.heap->size = len - sizeof(unit);
     *(unsigned int *)((char *)heap + len - sizeof(unit)) = 0;
}

void compact(void)
{
     msys.free = __compact(msys.heap, 0xffff);
}

#if 0
void *realloc(void *ptr, unsigned int size)
{
    void *p = malloc(size);
    memcpy(p, ptr, size); /* this is suboptimal, but not erroneous */
    free(ptr);
    return p;
}
#endif