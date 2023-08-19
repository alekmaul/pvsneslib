## Tips for the kit and possibly bugs

This page contains all tips that I found to use snes sdk compilers / linkers. It also contains possibly bugs I found when debugging examples and games I made with PVSnesLib.

## Tips

### Managing big files for graphics / sounds, and so on.

For big graphics, music, level data, etc. files, the best way to use them is to add them in assembly files rather than converting the data files to C arrays (see data.asm files in examples).  

If the files are larger than 32K, you need to split the files in the assembly file, the assembler will then manage it for you : 

```
.section ".rodatax" superfree
.incbin "big_file.bin" read $8000
.ends

.section ".rodatay" superfree
.incbin "big_file.bin" skip $8000 read $8000
.ends

; and so on... 
```

### Signedness

Handling signed types is a chore on the 65816. Use unsigned types wherever possible. If you are familiar with other processors, be informed that the 65816 is exceptionally inept, and handling signed types results in a larger performance impact than you might think. 

### Function Parameters

tcc-65816 passes function parameters on the stack, and - unlike other compilers - it does not align them. This means that you have to be extra careful to make sure that function prototypes are correct, because even minor deviations in type sizes wreak havoc, not to speak of prototypes missing altogether. 

### Pointers

The 65816 has a 24-bit address space divided into 256 banks with 64K of memory each. tcc-65816's pointers are 32-bit types of which the most significant byte is always zero. Pointer arithmetic is 16-bit. This has a couple of consequences:  

* It is not possible to cross bank boundaries using pointer arithmetic (or array indices, for that matter). In general, data structures crossing bank boundaries are not supported.  
   
* Conversion from pointers to scalars loses the bank information. (The only type, apart from pointers, that would be able to contain a complete 65816 long pointer is (unsigned) long long, but tcc-65816 is not currently able to convert between pointers and long longs. This is a bug, not a feature.)  
    
* Conversion from scalars to pointers results in a bank 0 pointer.  
    
* Writing a pointer to memory writes 32 bits. This can be a problem when writing to hardware registers or data structures that expect pointers to be 24 bits in size.  

One solution to many of these problems is to use a union for pointer conversion:  

```
union ptr {
  struct {
    unsigned short addr;
    unsigned char bank;
    unsigned char __pad;
  } c;
  void *p;
};
```

### __attribute__((packed)) doesn't work

```
struct dma_transfer { 
  unsigned short src_addr; /* 0 + 1 / 
  unsigned char src_bank; / 2 / 
  unsigned short dest; / 3 + 4 / 
  unsigned short size; / 5 + 6 / 
  unsigned char type; / 7 */ 
} 
attribute((packed));
```

This structure is 10 bytes instead of 8. (Is there a point in aligning stuff on the 65816 at all?). This is because tcc doesn't support it.

What you can do is to set attribute((packed)) on all members of the struct to solve it.

### Shiru's tips

Here are some others tips that [Shiru](http://shiru.untergrund.net/articles/programming_nes_games_in_c.htm) wrote about cc65 compiler but they can also applied to 816-tcc compiler.  
* Ordered List Item
* Avoid local variables as much as possible, make them static at least 
* Avoid passing parameters to functions 
* Avoid to use functions that only used once 
* Arrays of structs are slow, separate arrays are faster 
* Fastest type is unsigned char, use it as much as possible. Don't forget that in CC65 (and also 816-tcc) int is 16 bit wide 
* Signed types are slower 
* Don't forget that you need to declare array of pointers as const type* const if you need to put it into ROM 
* Use preincrements where possible, they are both faster and shorter 
* Avoid to use multiple and division as much as possible, they are very slow. Use bit shifts where possible instead 
* If you need to process an array of objects, it is better to copy data from arrays to separate vars. Use these vars in the code, and then copy new data back to the arrays. This could make code significally shorter and faster, because access to an array item generates more code than access to a variable 
* Declaring global variables static also helps to find unused global variables, compiler will report about them  

## Bugs

### The ! operator

In some cases, the **!** operator is not correctly working, use **== 0** instead of it in most cases.

Rather than :
```
    if (!(c & mask1)) {
    ..
    ;bla bla
```

Use :
```
    if ((c & mask1) == 0) {
    ..
    ;bla bla
```
