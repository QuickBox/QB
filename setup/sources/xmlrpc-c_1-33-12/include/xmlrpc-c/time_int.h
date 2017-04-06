#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <time.h>

#include "xmlrpc_config.h"
#include "xmlrpc-c/util.h"
#include "int.h"

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

#if HAVE_TIMESPEC
  # include <sys/time.h> /* for struct timespec */
  typedef struct timespec xmlrpc_timespec;
#else
  typedef struct {
      uint32_t tv_sec;
      uint32_t tv_nsec;
  } xmlrpc_timespec;
#endif

XMLRPC_UTIL_EXPORTED
void
xmlrpc_gettimeofday(xmlrpc_timespec * const todP);

XMLRPC_UTIL_EXPORTED
void
xmlrpc_timegm(const struct tm  * const brokenTime,
              time_t *           const timeValueP,
              const char **      const errorP);

XMLRPC_UTIL_EXPORTED
void
xmlrpc_localtime(time_t      const datetime,
                 struct tm * const tmP);

XMLRPC_UTIL_EXPORTED
void
xmlrpc_gmtime(time_t      const datetime,
              struct tm * const resultP);

#endif
