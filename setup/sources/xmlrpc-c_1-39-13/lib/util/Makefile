###############################################################################
# This directory builds object modules that provide utility functions that
# programs can use.  Not libraries, though -- just programs.  The reason
# we don't want any library to use object modules in here is that they'll
# probably pollute the name space when users link those libraries to their
# programs.  In fact, if more than one Xmlrpc-c library included one of these
# modules, the libraries would conflict with each other.
#
# So a utility function that is to be used by libraries (and, optionally,
# programs) should go in libxmlrpc_util.  libxmlrpc_util is a prerequisite
# for many Xmlrpc-c libraries, gets included in a program link only once,
# and uses external symbol names that have the "xmlrpc_" prefix to avoid
# collision with users' code.
#
# If we knew a portable way to link multiple object modules into one and
# restrict the symbols exported by the whole, we could avoid this mess and
# just link utility object modules into each Xmlrpc-c library.
##############################################################################

ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  LIBDIR := $(call updir,$(CURDIR))
  SRCDIR := $(call updir,$(LIBDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := lib/util

default: all

include $(BLDDIR)/config.mk

OMIT_UTILS_RULE = Y

include $(SRCDIR)/common.mk

# This 'common.mk' dependency makes sure the symlinks get built before
# this make file is used for anything.

$(SRCDIR)/common.mk: srcdir blddir

LIBOBJS = \
  casprintf.o \
  cmdline_parser.o \
  getoptx.o \
  string_parser.o \
  stripcaseeq.o \

# If the overall build is not building C++ libraries, then it isn't building
# any tools that use them, and it's a good assumption that it isn't building
# any program written in C++.  Since the main reason for not building C++
# libraries is that your build system can't compile C++, it makes sense not to
# attempt to compile any C++ code if we're not building C++ libraries.  Hence,
# we don't build any C++ code here if ENABLE_CPLUSPLUS is "no".

ifeq ($(ENABLE_CPLUSPLUS),yes)
  LIBOBJS += cmdline_parser_cpp.o
endif

.PHONY: all
all: $(LIBOBJS)

INCLUDES = -Isrcdir/$(SUBDIR)/include -I$(BLDDIR)

%.o:%.c
	$(CC) -c $(CFLAGS_ALL) $<

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS_ALL) $<

include depend.mk

.PHONY: clean distclean
clean: clean-common

distclean: clean distclean-common

.PHONY: dep
dep: dep-common

install: