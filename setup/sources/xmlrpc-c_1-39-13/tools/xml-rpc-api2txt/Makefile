ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  TOOLSDIR := $(call updir,$(CURDIR))
  SRCDIR := $(call updir,$(TOOLSDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := tools/xml-rpc-api2txt

default: all

include $(BLDDIR)/config.mk

PROGS :=
PROGRAMS_TO_INSTALL := xml-rpc-api2txt
MAN_FILES_TO_INSTALL := xml-rpc-api2txt.1

all: $(PROGS)

include $(SRCDIR)/tools/common.mk

# This common.mk dependency makes sure the symlinks get built before
# this make file is used for anything.

$(SRCDIR)/tools/common.mk: srcdir blddir

include depend.mk

.PHONY: clean
clean: clean-common
	rm -f $(PROGS)

.PHONY: distclean
distclean: clean distclean-common

.PHONY: dep
dep: dep-common
