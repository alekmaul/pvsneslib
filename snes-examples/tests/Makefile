# PVSNESLIB Test Suite Makefile

SUBDIRS := $(shell ls -d */ 2>/dev/null | sed 's|/||')

.PHONY: all clean test

all:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;

clean:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i clean || { exit 1;} fi; done;

test:
	./run_tests.sh
