#ifndef ABYSS_HANDER_H_INCLUDED
#define ABYSS_HANDER_H_INCLUDED

#include "xmlrpc-c/abyss.h"
#include "xmlrpc-c/server.h"
#include "xmlrpc-c/server_abyss.h"

struct uriHandlerXmlrpc {
/*----------------------------------------------------------------------------
   This is the part of an Abyss HTTP request handler (aka URI handler)
   that is specific to the Xmlrpc-c handler.
-----------------------------------------------------------------------------*/
    xmlrpc_registry *       registryP;
    const char *            uriPath;  /* malloc'ed */
    bool                    chunkResponse;
        /* The handler should chunk its response whenever possible */
    xmlrpc_call_processor * xmlProcessor;
    void *                  xmlProcessorArg;
    ResponseAccessCtl       accessControl;
};


void
xmlrpc_termUriHandler(void * const arg);

void
xmlrpc_handleIfXmlrpcReq(void *       const handlerArg,
                         TSession *   const abyssSessionP,
                         abyss_bool * const handledP);

abyss_bool
xmlrpc_serverAbyssDefaultUriHandler(TSession * const sessionP);

void
xmlrpc_initAccessCtl(ResponseAccessCtl * const accessCtlP,
                     const char *        const allowOrigin,
                     bool                const expires,
                     unsigned int        const maxAge);

void
xmlrpc_termAccessControl(ResponseAccessCtl * const accessCtlP);

void
xmlrpc_abyss_handler_trace(const char * const trace);

unsigned int
xmlrpc_abyss_handler_stacksize(void);

#endif
