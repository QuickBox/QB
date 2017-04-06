#ifndef LOCK_PLATFORM_H_INCLUDED
#define LOCK_PLATFORM_H_INCLUDED

#include "xmlrpc-c/lock.h"

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

#ifdef __cplusplus
extern "C" {
#endif

XMLRPC_UTIL_EXPORTED
lock *
xmlrpc_lock_create(void);

#ifdef __cplusplus
}
#endif

#endif
