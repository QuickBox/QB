ifeq ($(SRCDIR),)
  updir = $(shell echo $(dir $(1)) | sed 's/.$$//')
  SRCDIR := $(call updir,$(CURDIR))
  BLDDIR := $(SRCDIR)
endif
SUBDIR := src

include $(BLDDIR)/config.mk

default: all

SUBDIRS =

ifeq ($(ENABLE_CPLUSPLUS),yes)
  SUBDIRS += cpp
endif

TARGET_LIBRARY_NAMES := libxmlrpc libxmlrpc_server

# We cannot build libxmlrpc_server_abyss if we can't also build Abyss,
# because we need libxmlrpc_abyss in order to build the proper runtime
# dependencies into libxmlrpc_server_abyss.

ifeq ($(ENABLE_ABYSS_SERVER),yes)
  TARGET_LIBRARY_NAMES += libxmlrpc_server_abyss
endif

ifeq ($(MUST_BUILD_CLIENT),yes)
  TARGET_LIBRARY_NAMES += libxmlrpc_client
endif

ifeq ($(ENABLE_CGI_SERVER),yes)
  TARGET_LIBRARY_NAMES += libxmlrpc_server_cgi
endif

STATIC_LIBRARIES_TO_INSTALL = $(TARGET_STATIC_LIBRARIES)

SHARED_LIBS_TO_BUILD := $(TARGET_LIBRARY_NAMES)
SHARED_LIBS_TO_INSTALL := $(TARGET_LIBRARY_NAMES)

# TRANSPORT_MODS is the list of modules that have to go into the client
# library to provide the client XML transport functions.

# TRANSPORT_LIBDEP is linker -l options to declare what libraries contain
# things to which the transport objects refer.  (like LIBxxx_LIBDEP --
# see below)

TRANSPORT_MODS =
TRANSPORT_LIBDEP =
TRANSPORT_INCLUDES =
ifeq ($(MUST_BUILD_WININET_CLIENT),yes)
  TRANSPORT_MODS += $(BLDDIR)/lib/wininet_transport/xmlrpc_wininet_transport
  TRANSPORT_LIBDEP += $(shell wininet-config --libs)
  TRANSPORT_INCLUDES += -Isrcdir/lib/wininet_transport
endif
ifeq ($(MUST_BUILD_CURL_CLIENT),yes)
  TRANSPORT_MODS += $(BLDDIR)/lib/curl_transport/xmlrpc_curl_transport
  TRANSPORT_MODS += $(BLDDIR)/lib/curl_transport/curltransaction
  TRANSPORT_MODS += $(BLDDIR)/lib/curl_transport/curlmulti
  TRANSPORT_LIBDEP += $(shell curl-config --libs)
  TRANSPORT_INCLUDES += -Isrcdir/lib/curl_transport
endif
ifeq ($(MUST_BUILD_LIBWWW_CLIENT),yes)
  TRANSPORT_MODS += $(BLDDIR)/lib/libwww_transport/xmlrpc_libwww_transport
  TRANSPORT_LIBDEP += $(shell libwww-config --libs)
  TRANSPORT_INCLUDES += -Isrcdir/lib/libwww_transport
endif

ifeq ($(ENABLE_LIBXML2_BACKEND),yes)
  XMLRPC_XML_PARSER = xmlrpc_libxml2
  XML_PARSER_LIBDEP = $(shell xml2-config --libs)
  XML_PARSER_LIBDEP_DEP =
else
  XMLRPC_XML_PARSER = xmlrpc_expat
  XML_PARSER_LIBDEP = \
    -Lblddir/lib/expat/xmlparse -lxmlrpc_xmlparse \
    -Lblddir/lib/expat/xmltok -lxmlrpc_xmltok
  XML_PARSER_LIBDEP_DEP = $(LIBXMLRPC_XMLPARSE) $(LIBXMLRPC_XMLTOK)
endif

# LIBxxx_OBJS is the list of object files that make up library libxxx.

LIBXMLRPC_MODS = \
	base_global \
        double \
	json \
	parse_datetime \
	parse_value \
        resource \
	trace \
	version \
	xmlrpc_data \
	xmlrpc_datetime \
	xmlrpc_string \
	xmlrpc_array \
	xmlrpc_struct \
	xmlrpc_build \
	xmlrpc_decompose \
	$(XMLRPC_XML_PARSER) \
	xmlrpc_parse \
	xmlrpc_serialize \
	xmlrpc_authcookie \

LIBXMLRPC_CLIENT_MODS = xmlrpc_client xmlrpc_client_global xmlrpc_server_info

LIBXMLRPC_SERVER_MODS = registry method system_method

LIBXMLRPC_SERVER_ABYSS_MODS = xmlrpc_server_abyss abyss_handler

LIBXMLRPC_SERVER_CGI_MODS = xmlrpc_server_cgi

TARGET_MODS = \
  $(LIBXMLRPC_MODS) \
  $(LIBXMLRPC_SERVER_MODS) \
  $(LIBXMLRPC_SERVER_ABYSS_MODS) \
  $(LIBXMLRPC_SERVER_CGI_MODS) \
  $(LIBXMLRPC_CLIENT_MODS) \

OMIT_XMLRPC_LIB_RULE=Y
MAJ=3
  # Major number of shared libraries in this directory

include $(SRCDIR)/common.mk

# This 'common.mk' dependency makes sure the symlinks get built before
# this make file is used for anything.

$(SRCDIR)/common.mk: srcdir blddir

# TARGET_STATIC_LIBRARIES, etc. are set by common.mk, based on
# TARGET_LIBRARY_NAMES.


all: \
  $(TARGET_STATIC_LIBRARIES) \
  $(TARGET_SHARED_LIBRARIES) \
  $(TARGET_SHARED_LE_LIBS) \
  $(SUBDIRS:%=%/all) \

# Extra dependencies to make parallel make work in spite of all the submakes
# (See top level make file for details)

cpp/all: $(TARGET_SHARED_LE_LIBS)

ifeq ($MUST_BUILD_CLIENT),yes)
  cpp/all: $(BLDDIR)/transport_config.h
endif

#-----------------------------------------------------------------------------
#  RULES TO LINK LIBRARIES
#-----------------------------------------------------------------------------

# Rules for these are in common.mk, courtesy of TARGET_LIBRARY_NAMES:

# shlibfn generates e.g. libxmlrpc.so.3.1
# shliblefn generates e.g. libxmlrpc.so

LIBXMLRPC = $(call shlibfn, libxmlrpc)

$(LIBXMLRPC): $(LIBXMLRPC_MODS:%=%.osh) \
  $(LIBXMLRPC_UTIL) \
  $(XML_PARSER_LIBDEP_DEP)
$(LIBXMLRPC): LIBOBJECTS = $(LIBXMLRPC_MODS:%=%.osh)
$(LIBXMLRPC): LIBDEP = \
  $(LIBXMLRPC_UTIL_LIBDEP) \
  $(XML_PARSER_LIBDEP)

LIBXMLRPC_SERVER = $(call shlibfn, libxmlrpc_server)

$(LIBXMLRPC_SERVER): \
  $(LIBXMLRPC_SERVER_MODS:%=%.osh) \
  $(LIBXMLRPC_UTIL) \
  $(call shliblefn, libxmlrpc)
$(LIBXMLRPC_SERVER): LIBOBJECTS = $(LIBXMLRPC_SERVER_MODS:%=%.osh)
$(LIBXMLRPC_SERVER): LIBDEP = \
   -L. -lxmlrpc $(XML_PARSER_LIBDEP) $(LIBXMLRPC_UTIL_LIBDEP)

LIBXMLRPC_SERVER_ABYSS = $(call shlibfn, libxmlrpc_server_abyss)

$(LIBXMLRPC_SERVER_ABYSS): \
  $(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.osh) \
  $(LIBXMLRPC_UTIL) \
  $(LIBXMLRPC_ABYSS) \
  $(call shliblefn, libxmlrpc_server) \
  $(call shliblefn, libxmlrpc)
$(LIBXMLRPC_SERVER_ABYSS): LIBOBJECTS = $(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.osh)
$(LIBXMLRPC_SERVER_ABYSS): LIBDEP = \
  -L. -lxmlrpc_server \
  -L$(LIBXMLRPC_ABYSS_DIR) -lxmlrpc_abyss \
  -L. -lxmlrpc $(XML_PARSER_LIBDEP) $(LIBXMLRPC_UTIL_LIBDEP)
ifeq ($(MSVCRT),yes)
  $(LIBXMLRPC_SERVER_ABYSS):  LIBDEP += -lws2_32 -lwsock32
endif

LIBXMLRPC_SERVER_CGI = $(call shlibfn, libxmlrpc_server_cgi)

$(LIBXMLRPC_SERVER_CGI): \
  $(LIBXMLRPC_SERVER_CGI_MODS:%=%.osh) \
  $(LIBXMRPC_UTIL) \
  $(call shliblefn, libxmlrpc_server) \
  $(call shliblefn, libxmlrpc)
$(LIBXMLRPC_SERVER_CGI): LIBOBJECTS = $(LIBXMLRPC_SERVER_CGI_MODS:%=%.osh)
$(LIBXMLRPC_SERVER_CGI): LIBDEP = \
  -L. -lxmlrpc_server \
  -L. -lxmlrpc $(XML_PARSER_LIBDEP) $(LIBXMLRPC_UTIL_LIBDEP)

LIBXMLRPC_CLIENT = $(call shlibfn, libxmlrpc_client)

$(LIBXMLRPC_CLIENT): \
  $(LIBXMLRPC_CLIENT_MODS:%=%.osh) \
  $(TRANSPORT_MODS:%=%.osh) \
  $(LIBXMLRPC_UTIL) \
  $(call shliblefn, libxmlrpc)
$(LIBXMLRPC_CLIENT): LIBOBJECTS = \
  $(LIBXMLRPC_CLIENT_MODS:%=%.osh) \
  $(TRANSPORT_MODS:%=%.osh)

# We try to get Xmlrpc-c directories early in the link library search
# path to avert problems with other versions of Xmlrpc-c being in more
# general directories (such as /usr/local/lib) that are added to the
# search path by curl-config, etc.  That's why we separate the -L from
# the corresponding -l.
# 
# Note that in a properly configured system, curl-config, etc. do not
# generate -L options for general directories.

LIBXMLRPC_CLIENT_LIBDEP = \
  -Lblddir/src -Lblddir/lib/libutil \
  -lxmlrpc -lxmlrpc_util \
  $(XML_PARSER_LIBDEP) \
  $(TRANSPORT_LIBDEP) \

$(LIBXMLRPC_CLIENT): LIBDEP = $(LIBXMLRPC_CLIENT_LIBDEP)
ifeq ($(MSVCRT),yes)
  $(LIBXMLRPC_CLIENT):  LIBDEP += -lws2_32 -lwsock32 -lpthread
endif

libxmlrpc.a: $(LIBXMLRPC_MODS:%=%.o)
libxmlrpc.a: LIBOBJECTS = $(LIBXMLRPC_MODS:%=%.o)

libxmlrpc_server.a: $(LIBXMLRPC_SERVER_MODS:%=%.o)
libxmlrpc_server.a: LIBOBJECTS = $(LIBXMLRPC_SERVER_MODS:%=%.o)

libxmlrpc_server_abyss.a: $(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.o)
libxmlrpc_server_abyss.a: LIBOBJECTS=$(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.o)

libxmlrpc_server_cgi.a: $(LIBXMLRPC_SERVER_CGI_MODS:%=%.o)
libxmlrpc_server_cgi.a: LIBOBJECTS=$(LIBXMLRPC_SERVER_CGI_MODS:%=%.o)

libxmlrpc_client.a: $(LIBXMLRPC_CLIENT_MODS:%=%.o) $(TRANSPORT_MODS:%=%.o)
libxmlrpc_client.a: LIBOBJECTS = \
  $(LIBXMLRPC_CLIENT_MODS:%=%.o) \
  $(TRANSPORT_MODS:%=%.o)


#-----------------------------------------------------------------------------
#  RULES TO COMPILE OBJECT MODULES FOR LIBRARIES
#-----------------------------------------------------------------------------

# Rules for these are in common.mk, courtesy of TARGET_MODS:

BASIC_INCLUDES = \
  -Iblddir \
  -Iblddir/include \
  -Isrcdir/include \
  -Isrcdir/lib/util/include \

ifeq ($(ENABLE_LIBXML2_BACKEND),yes)
  LIBXML_INCLUDES = $(shell xml2-config --cflags)
else
  LIBXML_INCLUDES = -Isrcdir/lib/expat/xmlparse
endif

$(LIBXMLRPC_MODS:%=%.o) \
$(LIBXMLRPC_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES) $(LIBXML_INCLUDES)

$(LIBXMLRPC_CLIENT_MODS:%=%.o) \
$(LIBXMLRPC_CLIENT_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES) $(TRANSPORT_INCLUDES)

$(LIBXMLRPC_SERVER_MODS:%=%.o) \
$(LIBXMLRPC_SERVER_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES)

$(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.o) \
$(LIBXMLRPC_SERVER_ABYSS_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES) -Isrcdir/lib/abyss/src

$(LIBXMLRPC_SERVER_CGI_MODS:%=%.o) \
$(LIBXMLRPC_SERVER_CGI_MODS:%=%.osh): \
  INCLUDES = $(BASIC_INCLUDES)

#-----------------------------------------------------------------------------
#  RULES TO MAKE CFLAGS/LDFLAGS FILES
#
#  These are for builds in other directories of things that use libraries in
#  this directory.  E.g. when you build libxmlrpc_client++, it depends upon
#  libxmlrpc_client from this directory.  So the link of libxmlrpc_client++
#  must specify a dependency upon libxmlrpc_client and any library upon which
#  libxmlrpc_client depends, so the link command for libxmlrpc_client++
#  $(shell cat libxmlrpc_client.ldflags).
#  
#  Note that some systems don't need this because there is information in
#  libxmlrpc_client that tells libxmlrpc_client's dependencies, so you need
#  only find libxmlrpc_client to generate libxmlrpc_client++.
#-----------------------------------------------------------------------------

libxmlrpc_client.cflags:
	echo "$(TRANSPORT_INCLUDES)" >$@

libxmlrpc_client.ldflags:
	echo "-Lblddir/src -lxmlrpc_client $(LIBXMLRPC_CLIENT_LIBDEP)" >$@


#-----------------------------------------------------------------------------
.PHONY: check
check:

.PHONY: install
install: install-common $(SUBDIRS:%=%/install)

.PHONY: clean clean-local distclean distclean-local 
clean:  $(SUBDIRS:%=%/clean) clean-common clean-local
clean-local:

distclean: $(SUBDIRS:%=%/distclean) \
  clean-common \
  clean-local \
  distclean-local \
  distclean-common \

distclean-local:
	rm -f *.cflags *.ldflags

.PHONY: dep
dep: $(SUBDIRS:%=%/dep) $(BLDDIR)/transport_config.h dep-common

dep-common:
  INCLUDES = $(BASIC_INCLUDES) $(TRANSPORT_INCLUDES) $(LIBXML_INCLUDES)

xmlrpc_client.o xmlrpc_client.osh: $(BLDDIR)/transport_config.h
xmlrpc_client.o xmlrpc_client.osh: $(BLDDIR)/version.h
registry.o registry.osh: $(BLDDIR)/version.h
version.o version.osh: $(BLDDIR)/version.h

include depend.mk
