#!/bin/bash

# create all the opcode decoding speedup tables

export WLA_QUIET=1

make clean
export WLA_TARGET=GB
make
./gen

make clean
export WLA_TARGET=Z80
make
./gen

make clean
export WLA_TARGET=MCS6502
make
./gen

make clean
export WLA_TARGET=WDC65C02
make
./gen

make clean
export WLA_TARGET=MCS6510
make
./gen

make clean
export WLA_TARGET=W65816
make
./gen

make clean
export WLA_TARGET=SPC700
make
./gen

make clean
export WLA_TARGET=HUC6280
make
./gen

make clean
