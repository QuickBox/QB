ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  srcDIR := $(call updir,$(CURDIR))
  SRCDIR := $(call updir,$(srcDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := lib/libutil++

include $(BLDDIR)/config.mk

default: all

TARGET_LIB_NAMES_PP = libxmlrpc_util++

STATIC_LIBRARIES_TO_INSTALL = $(TARGET_STATIC_LIBRARIES)

SHARED_LIBS_TO_BUILD = $(TARGET_LIB_NAMES_PP)
SHARED_LIBS_TO_INSTALL = $(TARGET_LIB_NAMES_PP)

# INCLUDES and DEP_SOURCES are used by dep-common target
INCLUDES = $(BASIC_INCLUDES)
DEP_SOURCES = *.cpp

LIBXMLRPC_UTILPP_MODS = Lock base64 env_wrap girerr girmem

TARGET_MODS_PP = \
  $(LIBXMLRPC_UTILPP_MODS) \

OMIT_LIBXMLRPC_UTILPP_RULE = Y
MAJ = 8
  # Major number of shared libraries in this directory

include $(SRCDIR)/common.mk


# This 'common.mk' dependency makes sure the symlinks get built before
# this make file is used for anything.

$(SRCDIR)/common.mk: srcdir blddir

BASIC_INCLUDES = \
  -Iblddir/include \
  -Isrcdir/include \
  -Iblddir \
  -Isrcdir/lib/util/include


# TARGET_STATIC_LIBARIES, etc. are set by common.mk, based on
# TARGET_LIB_NAMES_PP.

all: \
  $(TARGET_STATIC_LIBRARIES) \
  $(TARGET_SHARED_LIBS_PP) \
  $(TARGET_SHARED_LE_LIBS) \


#-----------------------------------------------------------------------------
#  RULES TO LINK LIBRARIES
#-----------------------------------------------------------------------------

# Rules for these are in common.mk, courtesy of TARGET_LIB_NAMES_PP:

# shlibfn generates e.g. libxmlrpc.so.3.1
# shliblefn generates e.g. libxmlrpc.so

# Sometimes -lpthread is necessary when link-editing a shared library that
# uses pthread services, and sometimes it is not.  We've seen links on
# Windows and AIX fail without it.  It seems to be a good idea in all cases,
# as it declares an actual dependency of the shared library.

LIBXMLRPC_UTILPP_SH = $(call shlibfn, libxmlrpc_util++)

$(LIBXMLRPC_UTILPP_SH): $(LIBXMLRPC_UTILPP_MODS:%=%.osh) \
  $(LIBXMLRPC_UTIL) \

$(LIBXMLRPC_UTILPP_SH): LIBOBJECTS = $(LIBXMLRPC_UTILPP_MODS:%=%.osh)
$(LIBXMLRPC_UTILPP_SH): LIBDEP = \
   -L$(LIBXMLRPC_UTIL_DIR) -lxmlrpc_util \
   $(THREAD_LIBS) \

libxmlrpc_util++.a: $(LIBXMLRPC_UTILPP_MODS:%=%.o)
libxmlrpc_util++.a: LIBOBJECTS = $(LIBXMLRPC_UTILPP_MODS:%=%.o)

#-----------------------------------------------------------------------------
#  RULES TO COMPILE OBJECT MODULES FOR LIBRARIES
#-----------------------------------------------------------------------------

# Rules for these are in common.mk, courtesy of TARGET_MODS_PP.


#-----------------------------------------------------------------------------
#  MISCELLANEOUS RULES
#-----------------------------------------------------------------------------

.PHONY: install
install: install-common

.PHONY: clean clean-local distclean distclean-local
clean: clean-common clean-local
clean-local:

distclean: clean distclean-local distclean-common

distclean-local:

.PHONY: dep
dep: dep-common

include depend.mk
