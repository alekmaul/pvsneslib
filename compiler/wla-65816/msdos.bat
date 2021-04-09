
cd wlab
copy makefile.msdos makefile
make clean
make
move wlab.exe ..\binaries
make clean
cd ..

cd wlad
copy makefile.msdos makefile
make clean
make
move wlad.exe ..\binaries
make clean
cd ..

copy makefiles\makefile.msdos.gb MAKEFILE
make clean
make
move wla-gb.exe binaries\
make clean

copy makefiles\makefile.msdos.z80 MAKEFILE
make
move wla-z80.exe binaries\
make clean

copy makefiles\makefile.msdos.6502 MAKEFILE
make
move wla-6502.exe binaries\
make clean

copy makefiles\makefile.msdos.65c02 MAKEFILE
make
move wla-65c02.exe binaries\
make clean

copy makefiles\makefile.msdos.6510 MAKEFILE
make
move wla-6510.exe binaries\
make clean

copy makefiles\makefile.msdos.65816 MAKEFILE
make
move wla-65816.exe binaries\
make clean

copy makefiles\makefile.msdos.spc700 MAKEFILE
make
move wla-spc700.exe binaries\
make clean

copy makefiles\makefile.msdos.huc6280 MAKEFILE
make
move wla-huc6280.exe binaries\
make clean

cd wlalink
copy makefile.msdos MAKEFILE
make clean
make
move wlalink.exe ..\binaries
make clean
cd ..
