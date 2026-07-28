#---------------------------------------------------------------------------------
ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

# no default hdr.asm generation
AUTOHDR := 0

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

all: pvsl_font_data.asm
	@echo copying pvsl_font asm in source ...
	@cp pvsl_font_data.asm ../source/

#---------------------------------------------------------------------------------
# Build asm readble version of font
pvsl_font_data.asm : pvsl_font.pic

pvsl_font.pic: pvsl_font.png
	@echo convert default font with no tile reduction ... $(notdir $@)
	$(GFXCONV) -q -s 8 -o 16 -u 16 -p -e 0 -i $<
	@mv pvsl_font.pal pvslfont_pal.bin
	@mv pvsl_font.pic pvslfont_pic.bin
	$(TXCONV) -ca pvslfont_pal.bin
	@cp pvslfont_pal.asm pvsl_font_data.asm
	$(TXCONV) -ca pvslfont_pic.bin
	@cat pvslfont_pic.asm >> pvsl_font_data.asm
	@echo copying pvsl_font asm in source ...
	@cp pvsl_font_data.asm ../source/
	@rm -f pvsl_font.inc pvslfont_pic.asm pvslfont_pal.asm pvslfont_pic.bin pvslfont_pal.bin pvsl_font_data.as

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f pvslfont_pic.asm pvslfont_pal.asm pvslfont_pic.bin pvslfont_pal.bin pvsl_font.inc pvsl_font_data.asm pvsl_font_data.as
