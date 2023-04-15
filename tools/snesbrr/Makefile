CXX := g++

SUBDIRS := base brr

.PHONY : all $(SUBDIRS)

all : $(SUBDIRS)

ifeq ($(findstring $(MAKECMDGOALS),$(SUBDIRS)),)

ifneq ($(strip $(MAKECMDGOALS)),)
.PHONY : $(MAKECMDGOALS)
$(MAKECMDGOALS) : $(SUBDIRS)
endif

$(SUBDIRS) :
	$(MAKE) -C $@ $(MAKECMDGOALS)

else

$(SUBDIRS) :
	$(MAKE) -C $@

endif


brr : base
