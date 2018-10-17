ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  LIBDIR := $(call updir,$(CURDIR))
  SRCDIR := $(call updir,$(LIBDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := lib/abyss++

include $(BLDDIR)/config.mk

default: all

TARGET_LIB_NAMES_PP = \
  libxmlrpc_abyss++ \

STATIC_LIBRARIES_TO_INSTALL = $(TARGET_STATIC_LIBRARIES)

SHARED_LIBS_TO_BUILD = $(TARGET_LIB_NAMES_PP)
SHARED_LIBS_TO_INSTALL = $(TARGET_LIB_NAMES_PP)

# INCLUDES and DEP_SOURCES are used by dep-common target
INCLUDES = $(BASIC_INCLUDES)
DEP_SOURCES = *.cpp

LIBXMLRPC_ABYSSPP_MODS = AbyssServer AbyssChanSwitch AbyssEnvironment

TARGET_MODS_PP = \
  $(LIBXMLRPC_ABYSSPP_MODS) \

OMIT_ABYSSPP_LIB_RULE = Y
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
  $(TARGET_SHARED_LE_LIBS)


#-----------------------------------------------------------------------------
#  RULE TO LINK LIBRARIES
#-----------------------------------------------------------------------------

# The actual rules for this is in common.mk, courtesy of TARGET_LIB_NAMES_PP

# shlibfn generates e.g. libxmlrpc.so.3.1
# shliblefn generates e.g. libxmlrpc.so

# Sometimes -lpthread is necessary when link-editing a shared library that
# uses pthread services, and sometimes it is not.  We've seen links on
# Windows and AIX fail without it.  It seems to be a good idea in all cases,
# as it declares an actual dependency of the shared library.

LIBXMLRPC_ABYSSPP_SH = $(call shlibfn, libxmlrpc_abyss++)

$(LIBXMLRPC_ABYSSPP_SH): $(LIBXMLRPC_ABYSSPP_MODS:%=%.osh) \
  $(LIBXMLRPC_ABYSS) \
  $(LIBXMLRPC_UTILPP) \
  $(LIBXMLRPC_UTIL) \

$(LIBXMLRPC_ABYSSPP_SH): LIBOBJECTS = $(LIBXMLRPC_ABYSSPP_MODS:%=%.osh)
$(LIBXMLRPC_ABYSSPP_SH): LIBDEP = \
   -L. -Lblddir/src \
   -L$(LIBXMLRPC_ABYSS_DIR) -lxmlrpc_abyss \
   -L$(LIBXMLRPC_UTILPP_DIR) -lxmlrpc_util++ \
   -L$(LIBXMLRPC_UTIL_DIR) -lxmlrpc_util \

ifeq ($(MSVCRT),yes)
  $(LIBXMLRPC_ABYSSPP_SH):  LIBDEP += -lws2_32 -lwsock32 -lpthread
endif

libxmlrpc_abyss++.a: $(LIBXMLRPC_ABYSSPP_MODS:%=%.o)
libxmlrpc_abyss++.a: LIBOBJECTS=$(LIBXMLRPC_ABYSSPP_MODS:%=%.o)

#-----------------------------------------------------------------------------
#  RULE TO COMPILE OBJECT MODULES FOR LIBRARY
#-----------------------------------------------------------------------------

# The actual rule for this is in common.mk, courtesy of TARGET_MODS_PP

$(LIBXMLRPC_ABYSSPP_MODS:%=%.o) \
$(LIBXMLRPC_ABYSSPP_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES)

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
dep: dep-common $(BLDDIR)/transport_config.h
dep-common: $(BLDDIR)/src/libxmlrpc_client.cflags

include depend.mk
