SUBDIRS:= $(shell ls | egrep -v '^(CVS)$$')

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

#---------------------------------------------------------------------------------
all: examples 
#---------------------------------------------------------------------------------
	@rm -fr bin
	@mkdir -p bin
	find . -name "*.sfc" -exec cp -fv {} bin \;

examples:
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE) -C $$i || { exit 1;} fi; done;

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	@rm -fr bin
	@rm -f *.bz2
	@for i in $(SUBDIRS); do if test -e $$i/Makefile ; then $(MAKE)  -C $$i clean || { exit 1;} fi; done;

