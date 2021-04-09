#!/bin/bash

# create all the mnemonic lists

# GB

make clean
export WLA_TARGET=GB
export WLA_NICELIST=NICELIST1
make
./op | sort > _gb_a

make clean
export WLA_TARGET=GB
export WLA_NICELIST=NICELIST2
make
./op | sort > _gb_o

# Z80

make clean
export WLA_TARGET=Z80
export WLA_NICELIST=NICELIST1
make
./op | sort > _z80_a

make clean
export WLA_TARGET=Z80
export WLA_NICELIST=NICELIST2
make
./op | sort > _z80_o

# 6502

make clean
export WLA_TARGET=MCS6502
export WLA_NICELIST=NICELIST1
make
./op | sort > _6502_a

make clean
export WLA_TARGET=MCS6502
export WLA_NICELIST=NICELIST2
make
./op | sort > _6502_o

# 65c02

make clean
export WLA_TARGET=WDC65C02
export WLA_NICELIST=NICELIST1
make
./op | sort > _65c02_a

make clean
export WLA_TARGET=WDC65C02
export WLA_NICELIST=NICELIST2
make
./op | sort > _65c02_o

# 6510

make clean
export WLA_TARGET=MCS6510
export WLA_NICELIST=NICELIST1
make
./op | sort > _6510_a

make clean
export WLA_TARGET=MCS6510
export WLA_NICELIST=NICELIST2
make
./op | sort > _6510_o

# 65816

make clean
export WLA_TARGET=W65816
export WLA_NICELIST=NICELIST1
make
./op | sort > _65816_a

make clean
export WLA_TARGET=W65816
export WLA_NICELIST=NICELIST2
make
./op | sort > _65816_o

# SPC-700

make clean
export WLA_TARGET=SPC700
export WLA_NICELIST=NICELIST1
make
./op | sort > _spc700_a

make clean
export WLA_TARGET=SPC700
export WLA_NICELIST=NICELIST2
make
./op | sort > _spc700_o

# HUC6280

make clean
export WLA_TARGET=HUC6280
export WLA_NICELIST=NICELIST1
make
./op | sort > _huc6280_a

make clean
export WLA_TARGET=HUC6280
export WLA_NICELIST=NICELIST2
make
./op | sort > _huc6280_o

make clean

# create the final files

# GB

cat header_a_gb.txt _gb_a > mnemonics_gb_a.txt
cat header_o_gb.txt _gb_o > mnemonics_gb_o.txt

# Z80

cat header_a_z80.txt _z80_a > mnemonics_z80_a.txt
cat header_o_z80.txt _z80_o > mnemonics_z80_o.txt

# 6502

cat header_a_6502.txt _6502_a > mnemonics_6502_a.txt
cat header_o_6502.txt _6502_o > mnemonics_6502_o.txt

# 65c02

cat header_a_65c02.txt _65c02_a > mnemonics_65c02_a.txt
cat header_o_65c02.txt _65c02_o > mnemonics_65c02_o.txt

# 6510

cat header_a_6510.txt _6510_a > mnemonics_6510_a.txt
cat header_o_6510.txt _6510_o > mnemonics_6510_o.txt

# 65816

cat header_a_65816.txt _65816_a > mnemonics_65816_a.txt
cat header_o_65816.txt _65816_o > mnemonics_65816_o.txt

# SPC-700

cat header_a_spc700.txt _spc700_a > mnemonics_spc700_a.txt
cat header_o_spc700.txt _spc700_o > mnemonics_spc700_o.txt

# HUC6280

cat header_a_huc6280.txt _huc6280_a > mnemonics_huc6280_a.txt
cat header_o_huc6280.txt _huc6280_o > mnemonics_huc6280_o.txt

# cleanup

rm _*
