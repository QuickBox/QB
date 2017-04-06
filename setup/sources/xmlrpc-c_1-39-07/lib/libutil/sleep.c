#include "xmlrpc_config.h"
#include "bool.h"

#include "xmlrpc-c/sleep_int.h"

#if MSVCRT
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include <process.h>
#else
#  include <unistd.h>
#endif


void
xmlrpc_millisecond_sleep(unsigned int const milliseconds) {

#if MSVCRT
    SleepEx(milliseconds, true);
#else
    usleep(milliseconds * 1000);
#endif
}
