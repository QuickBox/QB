/*============================================================================
  socket.c
==============================================================================
  Implementation of TChanSwitch class: A generic channel switch -- an object
  that brokers a connection between an HTTP client and server.
============================================================================*/

#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "int.h"
#include "mallocvar.h"
#include "xmlrpc-c/util_int.h"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/abyss.h"
#ifdef _WIN32
  #include "socket_win.h"
#else
  #include "socket_unix.h"
#endif
#include "chanswitch.h"


static void
socketOsInit(const char ** const errorP) {

#ifdef _WIN32
    SocketWinInit(errorP);
#else
    SocketUnixInit(errorP);
#endif
}



static void
socketOsTerm(void) {

#ifdef _WIN32
    SocketWinTerm();
#else
    SocketUnixTerm();
#endif
}
    


bool SwitchTraceIsActive;

void
ChanSwitchInit(const char ** const errorP) {

    socketOsInit(errorP);

    if (!*errorP) {
        SwitchTraceIsActive = (getenv("ABYSS_TRACE_SWITCH") != NULL);
        if (SwitchTraceIsActive)
            fprintf(stderr, "Abyss channel switch layer will trace "
                    "channel connection activity "
                    "because of ABYSS_TRACE_SWITCH environment variable\n");
    }
}



void
ChanSwitchTerm(void) {

    socketOsTerm();
}



/* ChanSwitchCreate() is not exported to the Abyss user.  It is meant to
   be used by an implementation-specific TChanSwitch generator which is
   exported to the Abyss user, e.g. SwitchCreateUnix() in
   socket_unix.c

   The TChanSwitch generator functions are the _only_ user-accessible
   functions that are particular to an implementation.
*/

static unsigned int const switchSignature = 0x06060A;

void
ChanSwitchCreate(const struct TChanSwitchVtbl * const vtblP,
                 void *                         const implP,
                 TChanSwitch **                 const chanSwitchPP) {

    TChanSwitch * chanSwitchP;

    MALLOCVAR(chanSwitchP);

    if (chanSwitchP) {
        chanSwitchP->implP     = implP;
        chanSwitchP->vtbl      = *vtblP;
        chanSwitchP->signature = switchSignature;

        if (SwitchTraceIsActive)
            fprintf(stderr, "Created channel switch %p\n", chanSwitchP);

        *chanSwitchPP = chanSwitchP;
    }
}



void
ChanSwitchDestroy(TChanSwitch * const chanSwitchP) {

    if (SwitchTraceIsActive)
        fprintf(stderr, "Destroying channel switch %p\n", chanSwitchP);

    assert(chanSwitchP->signature == switchSignature);

    chanSwitchP->vtbl.destroy(chanSwitchP);

    chanSwitchP->signature = 0;  /* For debuggability */

    free(chanSwitchP);
}



void
ChanSwitchListen(TChanSwitch * const chanSwitchP,
                 uint32_t      const backlog,
                 const char ** const errorP) {

    if (SwitchTraceIsActive)
        fprintf(stderr, "Channel switch %p listening.\n", chanSwitchP);

    (*chanSwitchP->vtbl.listen)(chanSwitchP, backlog, errorP);

    if (!*errorP)
        chanSwitchP->isListening = true;
}



void
ChanSwitchAccept(TChanSwitch * const chanSwitchP,
                 TChannel **   const channelPP,
                 void **       const channelInfoPP,
                 const char ** const errorP) {

    if (!chanSwitchP->isListening)
        xmlrpc_asprintf(errorP, "Attempt to accept next connection from a "
                        "channel switch that is not listening "
                        "for connections");
    else {
        if (SwitchTraceIsActive)
            fprintf(stderr, "Getting a connection from Channel switch %p...\n",
                    chanSwitchP);

        (*chanSwitchP->vtbl.accept)(chanSwitchP,
                                    channelPP, channelInfoPP, errorP);

        if (SwitchTraceIsActive)
            fprintf(stderr, "Got connection from channel switch.  "
                    "Channel = %p\n", *channelPP);
    }
}



void
ChanSwitchInterrupt(TChanSwitch * const chanSwitchP) {

    if (SwitchTraceIsActive)
        fprintf(stderr, "Interrupting wait for a connection "
                "by Channel switch %p...\n",
                chanSwitchP);

    (*chanSwitchP->vtbl.interrupt)(chanSwitchP);
}
