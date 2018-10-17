CLIENT_LDLIBS = $(shell cat blddir/src/libxmlrpc_client.ldflags) $(LDLIBS_XML)

CLIENT_LIBS_DEP = \
  $(LIBXMLRPC_CLIENT) \
  $(LIBXMLRPC) \
  $(LIBXMLRPC_XML) \
  $(LIBXMLRPC_UTIL) \
  $(BLDDIR)/src/libxmlrpc_client.ldflags \

CLIENTPP_LDLIBS = \
  -Lblddir/src/cpp \
  -lxmlrpc_client++ \
  -lxmlrpc_packetsocket \
  -lxmlrpc++ \
  -Lblddir/lib/libutil++ \
  -lxmlrpc_util++ \

include $(SRCDIR)/common.mk

ifneq ($(OMIT_LIB_RULE),Y)
blddir/tools/lib/dumpvalue.o: FORCE
	$(MAKE) -C $(dir $@) -f $(SRCDIR)/tools/lib/Makefile $(notdir $@) 
endif

.PHONY: install
install: install-common

.PHONY: check
check:

.PHONY: FORCE
FORCE:
