#---------------------------------------------------------------------------------
ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME with path to its folder and restart application. (you can do it on windows with <setx PVSNESLIB_HOME "/c/snesdev">)")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

export TA := tasm

all: sm_spc.asm
	@echo copying sm_spc.asm in source ...
	@cp sm_spc.asm ../source/

#---------------------------------------------------------------------------------
# just make a asm file of snesmod spc-700 driver
sm_spc.asm : sm_spc.as7
	$(TA) -07 -b -l $< sm_spc.obj sm_spc.lst
	$(TXCONV) -ca sm_spc.obj
	@rm sm_spc.obj sm_spc.lst
    
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f sm_spc.asm
