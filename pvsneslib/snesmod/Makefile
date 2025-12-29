#---------------------------------------------------------------------------------
ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

# WLA-DX SPC700 assembler and linker
export WLASPC := $(PVSNESLIB_HOME)/devkitsnes/bin/wla-spc700
export WLALINK := $(PVSNESLIB_HOME)/devkitsnes/bin/wlalink

all: sm_spc.asm
	@echo copying sm_spc.asm in source ...
	@cp sm_spc.asm ../source/

#---------------------------------------------------------------------------------
# Build SPC700 driver using WLA-DX (cross-platform)
sm_spc.asm : sm_spc_wla.asm sm_spc_wla.link
	$(WLASPC) -o sm_spc_wla.obj sm_spc_wla.asm
	$(WLALINK) -b sm_spc_wla.link sm_spc_tmp.bin
	@mv sm_spc_tmp.bin sm_spc.bin
	$(TXCONV) -ca sm_spc.bin
	@rm -f sm_spc_wla.obj sm_spc.bin

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f sm_spc.asm sm_spc_wla.obj sm_spc.bin
