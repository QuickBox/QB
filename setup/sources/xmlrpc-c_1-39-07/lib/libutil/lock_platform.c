/*=============================================================================
                              lock_platform
===============================================================================

  This module provides locking services appropriate for the platform for
  which Xmlrpc-c is being built.  I.e. services chosen by the build
  configuration.

============================================================================*/

#include "xmlrpc_config.h"

#include "xmlrpc-c/lock_platform.h"

#if HAVE_PTHREAD
#include "xmlrpc-c/lock_pthread.h"
#endif

#if HAVE_WINDOWS_THREAD
#include "xmlrpc-c/lock_windows.h"
#endif

struct lock *
xmlrpc_lock_create(void) {

#if HAVE_PTHREAD
    return xmlrpc_lock_create_pthread();
#elif HAVE_WINDOWS_THREAD
    return xmlrpc_lock_create_windows();
#else
  #error "You don't have any thread facility.  (According to "
  #error "HAVE_PTHREAD and HAVE_WINDOWS_THREAD macros defined in "
  #error "xmlrpc_config.h)"

  /* One might consider using xmlrpc_lock_create_none() here, but that would
     be dangerous.  If the system really does have threads that share memory,
     and there's just some configuration error here (which is the most likely
     case), we would silently build a broken library, which breakage would be
     difficult for the user to detect and then diagnose.

     If we encounter some actual need for lock_none in the future, we'll
     revisit the issue of when the build system should select it
  */
#endif

}


