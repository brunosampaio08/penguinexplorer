SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)
$(SUBDIRS):
	cd $@ && $(MAKE) -f *mk

.PHONY: all $(SUBDIRS)
