#ifndef benchmark_H_
#define benchmark_H_

#include <snes.h>

//------------------------------------------------------------------------------------------
extern void Benchmark_Start(void);
extern u16 Benchmark_End_Unified(void);

//------------------------------------------------------------------------------------------
extern u8 line;
extern u16 startsec,finishsec;

//------------------------------------------------------------------------------------------
//#define dotest(tstfct,tstname) { startsec=GetLinearTime(); tstfct();  finishsec=GetLinearTime(); tstPostResult(tstname); WaitForVBlank(); }
#define dotest(tstfct,tstname) { Benchmark_Start(); tstfct();  Benchmark_End(); tstPostResult(tstname); WaitForVBlank(); }


#endif