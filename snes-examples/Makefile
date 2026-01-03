SUBDIRS    := $(shell ls | grep -Ev '^(CVS|tests)$$')
DATESTRING := $(shell date +%Y%m%d)

.PHONY: all install clean

all:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;

install:
	@rm -fr bin
	@mkdir -p bin
	find . -name "*.sfc" -exec cp -fv {} bin \;

clean:
	@rm -fr bin
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i clean || { exit 1;} fi; done;

