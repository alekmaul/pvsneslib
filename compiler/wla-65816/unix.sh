#!/bin/bash

export CC='gcc'
export LD='gcc'

echo
echo "****************************************************************"
echo "*          creating opcode decoding speedup tables             *"
echo "****************************************************************"
echo

cd opcode_table_generator
./create_tables.sh
cd ..

echo
echo "****************************************************************"
echo "*                   compiling few tools                        *"
echo "****************************************************************"
echo

cd wlab
cp makefile.unix makefile
make
cp wlab ../binaries
make clean
cd ..

cd wlad
cp makefile.unix makefile
make
cp wlad ../binaries
make clean
cd ..

echo
echo "****************************************************************"
echo "*                  compiling WLA binaries                      *"
echo "****************************************************************"
echo

make clean
cp makefiles/makefile.unix.gb makefile
make
cp wla-gb binaries

make clean
cp makefiles/makefile.unix.z80 makefile
make
cp wla-z80 binaries

make clean
cp makefiles/makefile.unix.6502 makefile
make
cp wla-6502 binaries

make clean
cp makefiles/makefile.unix.65c02 makefile
make
cp wla-65c02 binaries

make clean
cp makefiles/makefile.unix.6510 makefile
make
cp wla-6510 binaries

make clean
cp makefiles/makefile.unix.65816 makefile
make
cp wla-65816 binaries

make clean
cp makefiles/makefile.unix.spc700 makefile
make
cp wla-spc700 binaries

make clean
cp makefiles/makefile.unix.huc6280 makefile
make
cp wla-huc6280 binaries

make clean

echo
echo "****************************************************************"
echo "*                    compiling WLALINK                         *"
echo "****************************************************************"
echo

cd wlalink
cp makefile.unix makefile
make
cp wlalink ../binaries
make clean
cd ..

echo
echo "****************************************************************"
echo "*        all done! check ./binaries/ for the results           *"
echo "****************************************************************"
echo
