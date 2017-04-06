#ifndef STRING_NUMBER_H_INCLUDED
#define STRING_NUMBER_H_INCLUDED

#include <xmlrpc-c/config.h>
#include <xmlrpc-c/util.h>

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
xmlrpc_parse_int64(xmlrpc_env *   const envP,
                   const char *   const str,
                   xmlrpc_int64 * const i64P);

#ifdef __cplusplus
}
#endif

#endif
