#define _GNU_SOURCE   /* But only when HAVE_ASPRINTF */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "xmlrpc_config.h"  /* For HAVE_ASPRINTF, __inline__ */
#include "bool.h"
#include "casprintf.h"



static __inline__ void
newVsnprintf(char *       const buffer,
             size_t       const bufferSize,
             const char * const fmt,
             va_list            varargs,
             size_t *     const formattedSizeP) {
/*----------------------------------------------------------------------------
   This is vsnprintf() with the new behavior, where not fitting in the buffer
   is not a failure.

   Unfortunately, we can't practically return the size of the formatted string
   if the C library has old vsnprintf() and the formatted string doesn't fit
   in the buffer, so in that case we just return something larger than the
   buffer.
-----------------------------------------------------------------------------*/
    if (bufferSize > INT_MAX/2) {
        /* There's a danger we won't be able to coerce the return value
           of XMLRPC_VSNPRINTF to an integer (which we have to do because,
           while for POSIX its return value is ssize_t, on Windows it is int),
           or return double the buffer size.
        */
        *formattedSizeP = 0;
    } else {
        int rc;

        rc = XMLRPC_VSNPRINTF(buffer, bufferSize, fmt, varargs);

        if (rc < 0) {
            /* We have old vsnprintf() (or Windows) and the formatted value
               doesn't fit in the buffer, but we don't know how big a buffer it
               needs.
            */
            *formattedSizeP = bufferSize * 2;
        } else {
            /* Either the string fits in the buffer or we have new vsnprintf()
               which tells us how big the string is regardless.
            */
            *formattedSizeP = rc;
        }
    }
}



static __inline__ int
simpleVasprintf(char **      const resultP,
                const char * const fmt,
                va_list            varargs) {
/*----------------------------------------------------------------------------
   This is a poor man's implementation of vasprintf(), of GNU fame.
-----------------------------------------------------------------------------*/
    int retval;
    char * buffer;
    size_t bufferSize;
    bool outOfMemory;

    for (buffer = NULL, bufferSize = 4096, outOfMemory = false;
         !buffer && !outOfMemory;
        ) {

        buffer = malloc(bufferSize);
        if (!buffer)
            outOfMemory = true;
        else {
            size_t bytesNeeded;
            newVsnprintf(buffer, bufferSize, fmt, varargs, &bytesNeeded);
            if (bytesNeeded > bufferSize) {
                free(buffer);
                buffer = NULL;
                bufferSize = bytesNeeded;
            }
        }
    }
    if (outOfMemory)
        retval = -1;
    else {
        retval = strlen(buffer);
        *resultP = buffer;
    }
    return retval;
}



const char * const strsol = "[Insufficient memory to build string]";



void
cvasprintf(const char ** const retvalP,
           const char *  const fmt,
           va_list             varargs) {
    
    char * string;
    int rc;

#if HAVE_ASPRINTF
    rc = vasprintf(&string, fmt, varargs);
#else
    rc = simpleVasprintf(&string, fmt, varargs);
#endif

    if (rc < 0)
        *retvalP = strsol;
    else
        *retvalP = string;
}



void GNU_PRINTF_ATTR(2,3)
casprintf(const char ** const retvalP, const char * const fmt, ...) {

    va_list varargs;  /* mysterious structure used by variable arg facility */

    va_start(varargs, fmt); /* start up the mysterious variable arg facility */

    cvasprintf(retvalP, fmt, varargs);

    va_end(varargs);
}



void
strfree(const char * const string) {

    if (string != strsol)
        free((void *)string);
}
