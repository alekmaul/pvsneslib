/*---------------------------------------------------------------------------------
	PVSnesLib mathematic benchmark test
	-- alekmaul
---------------------------------------------------------------------------------*/

#include "benchmark.h"

#define MAXLOOPMATH 200
u8 loop;
u16 tst1v1u16add,tst1v2u16add;
u32 tst1v2u32add,tst1v2u32add;
void initMathTest(void) {
    tst1v1u16add=0;tst1v1u16add=5;
    tst1v2u32add=0;tst1v2u32add=5;
}
void mathU16AddConst(void) {
    for (loop=0;loop<MAXLOOPMATH;loop++) {
        tst1v1u16add+=4;
    }
}
void mathU16AddVar(void) {
    for (loop=0;loop<MAXLOOPMATH;loop++) {
        tst1v1u16add+=tst1v1u16add;
    }
}
void mathU32AddConst(void) {
    for (loop=0;loop<MAXLOOPMATH;loop++) {
        tst1v2u32add+=4;
    }
}
void mathU32AddVar(void) {
    for (loop=0;loop<MAXLOOPMATH;loop++) {
        tst1v2u32add+=tst1v2u32add;
    }
}

void runMathTest(void){
    initMathTest();
    dotest(mathU16AddConst,"U16 ADD   4");
    dotest(mathU16AddVar,  "U16 ADD VAR");
    dotest(mathU32AddConst,"U32 ADD   4");
    dotest(mathU32AddVar,  "U32 ADD VAR");
}