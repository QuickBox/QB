#ifndef SLEEP_INT_H_INCLUDED
#define SLEEP_INT_H_INCLUDED

#include "xmlrpc-c/c_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  XMLRPC_UTIL_EXPORTED marks a symbol in this file that is exported from
  libxmlrpc_util.

  XMLRPC_BUILDING_UTIL says this compilation is part of libxmlrpc_util, as
  opposed to something that _uses_ libxmlrpc_util.
*/
#ifdef XMLRPC_BUILDING_UTIL
#define XMLRPC_UTIL_EXPORTED XMLRPC_DLLEXPORT
#else
#define XMLRPC_UTIL_EXPORTED
#endif

XMLRPC_UTIL_EXPORTED
void
xmlrpc_millisecond_sleep(unsigned int const milliseconds);

#ifdef __cplusplus
}
#endif

#endif
