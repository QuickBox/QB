ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  TOOLSDIR := $(call updir,$(CURDIR))
  SRCDIR := $(call updir,$(TOOLSDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := tools/xml

default: all

include $(BLDDIR)/config.mk

PROGS := xmlrpc_parsecall

PROGRAMS_TO_INSTALL = $(PROGS)

include $(SRCDIR)/tools/common.mk

INCLUDES = \
  -I../lib/include \
  -Iblddir \
  -Iblddir/include \
  -Isrcdir/include \
  -Isrcdir/lib/util/include

all: $(PROGS)

UTIL_OBJS = \
  casprintf.o \

UTILS = $(UTIL_OBJS:%=$(UTIL_DIR)/%)

DUMPVALUE = blddir/tools/lib/dumpvalue.o

# Can we just use $(LIBS) in the link?

LIBS = $(LIBXMLRPC) $(LIBXMLRPC_XML) $(LIBXMLRPC_UTIL)

PARSECALL_OBJS = xmlrpc_parsecall.o $(DUMPVALUE)

xmlrpc_parsecall: $(PARSECALL_OBJS) $(LIBS) $(UTILS)
	$(CCLD) -o $@ $(LDFLAGS_ALL) $^

%.o:%.c $(BLDDIR)/include/xmlrpc-c/config.h
	$(CC) -c $(CFLAGS_ALL) $<

# This common.mk dependency makes sure the symlinks get built before
# this make file is used for anything.

$(SRCDIR)/tools/common.mk: srcdir blddir

include depend.mk

.PHONY: dep
dep: dep-common

.PHONY: clean
clean: clean-common
	rm -f $(PROGS)

.PHONY: distclean
distclean: clean distclean-common
