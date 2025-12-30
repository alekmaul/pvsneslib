ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME")
endif

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all clean test

all: test_tileset.map

# Generate tileset with map file for tmx2snes
test_tileset.pic: test_tileset.png
	@echo convert tileset ... $(notdir $@)
	$(GFXCONV) -s 8 -o 48 -u 16 -p -m -i $<

test_tileset.map: test_tileset.pic

# Run the test
test: test_tileset.map
	./run_test.sh

clean:
	rm -f *.pic *.pal *.map *.m16 *.b16 *.t16
