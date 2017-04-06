/* Copyright information is at the end of the file */

#include "xmlrpc_config.h"

#define _XOPEN_SOURCE 600  /* For strdup(), sigaction */
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/abyss.h */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#ifdef _WIN32
#  include <io.h>
#  pragma comment(lib, "Ws2_32.lib")
#else
#  include <signal.h>
#  include <sys/wait.h>
#  include <grp.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#endif

#include "bool.h"
#include "int.h"
#include "mallocvar.h"

#include "xmlrpc-c/abyss.h"
#include "xmlrpc-c/base.h"
#include "xmlrpc-c/server.h"
#include "xmlrpc-c/base_int.h"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/server_abyss.h"

#include "abyss_handler.h"


struct xmlrpc_server_abyss {
    TServer       abyssServer;
    TChanSwitch * chanSwitchP;
    bool          shutdownEnabled;
        /* User wants system.shutdown to succeed */
};



static void
dieIfFaultOccurred(xmlrpc_env * const envP) {

    if (envP->fault_occurred) {
        fprintf(stderr, "Unexpected XML-RPC fault: %s (%d)\n",
                envP->fault_string, envP->fault_code);
        exit(1);
    }
}



static void
initAbyss(xmlrpc_env * const envP) {

    const char * error;
    AbyssInit(&error);
    if (error) {
        xmlrpc_faultf(envP, "Failed to initialize the Abyss library.  %s",
                      error);
        xmlrpc_strfree(error);
    }
}



static void
termAbyss(void) {

    AbyssTerm();
}



static unsigned int globallyInitialized = 0;
    /* Initialization count */


void
xmlrpc_server_abyss_global_init(xmlrpc_env * const envP) {
    
    /* Note that this is specified as not thread safe; user calls it at
       the beginning of his program, when it is only one thread.
    */

    XMLRPC_ASSERT_ENV_OK(envP);
    
    if (globallyInitialized == 0)
        initAbyss(envP);

    ++globallyInitialized;
}



void
xmlrpc_server_abyss_global_term(void) {

    /* Note that this is specified as not thread safe; user calls it at
       the end of his program, when it is only one thread.
    */

    XMLRPC_ASSERT(globallyInitialized > 0);

    --globallyInitialized;

    if (globallyInitialized == 0)
        termAbyss();
}



static void
validateGlobalInit(xmlrpc_env * const envP) {

    if (!globallyInitialized)
        xmlrpc_faultf(envP, "libxmlrpc_server_abyss has not been globally "
                      "initialized.  See xmlrpc_server_abyss_init()");
}



static void
processXmlrpcCall(xmlrpc_env *        const envP,
                  void *              const arg,
                  const char *        const callXml,
                  size_t              const callXmlLen,
                  TSession *          const abyssSessionP,                  
                  xmlrpc_mem_block ** const responseXmlPP) {

    xmlrpc_registry * const registryP = arg;

    xmlrpc_registry_process_call2(envP, registryP,
                                  callXml, callXmlLen, abyssSessionP,
                                  responseXmlPP);

}



static void
setHandler(xmlrpc_env *              const envP,
           TServer *                 const srvP,
           struct uriHandlerXmlrpc * const uriHandlerXmlrpcP,
           size_t                    const xmlProcessorMaxStackSize) {
    
    abyss_bool success;

    xmlrpc_abyss_handler_trace(
        getenv("XMLRPC_TRACE_ABYSS"));
                                 
    {
        size_t const stackSize = 
            xmlrpc_abyss_handler_stacksize() + xmlProcessorMaxStackSize;
        struct ServerReqHandler3 const handlerDesc = {
            /* .term               = */ &xmlrpc_termUriHandler,
            /* .handleReq          = */ &xmlrpc_handleIfXmlrpcReq,
            /* .userdata           = */ uriHandlerXmlrpcP,
            /* .handleReqStackSize = */ stackSize
        };
        ServerAddHandler3(srvP, &handlerDesc, &success);
    }
    if (!success)
        xmlrpc_faultf(envP, "Abyss failed to register the Xmlrpc-c request "
                      "handler.  ServerAddHandler3() failed.");
}



static void
interpretHttpAccessControl(
    const xmlrpc_server_abyss_handler_parms * const parmsP,
    unsigned int                              const parmSize,
    ResponseAccessCtl *                       const accessCtlP) {

    const char * allowOrigin;
    bool         expires;
    unsigned int maxAge;

    if (parmSize >= XMLRPC_AHPSIZE(allow_origin) && parmsP->allow_origin)
        allowOrigin = parmsP->allow_origin;
    else
        allowOrigin = NULL;

    if (parmSize >= XMLRPC_AHPSIZE(access_ctl_expires)
        && parmsP->access_ctl_expires) {
        expires = true;

        if (parmSize >= XMLRPC_AHPSIZE(access_ctl_max_age))
            maxAge = parmsP->access_ctl_max_age;
        else
            maxAge = 0;
    } else {
        expires = false;
        maxAge = 0;  /* Meaningless; just to quiet runtime memory checks */
    }

    xmlrpc_initAccessCtl(accessCtlP, allowOrigin, expires, maxAge);
}



void
xmlrpc_server_abyss_set_handler3(
    xmlrpc_env *                              const envP,
    TServer *                                 const srvP,
    const xmlrpc_server_abyss_handler_parms * const parmsP,
    unsigned int                              const parmSize) {

    struct uriHandlerXmlrpc * uriHandlerXmlrpcP;
    size_t xmlProcessorMaxStackSize;

    MALLOCVAR_NOFAIL(uriHandlerXmlrpcP);

    if (!envP->fault_occurred) {
        if (parmSize >= XMLRPC_AHPSIZE(xml_processor))
            uriHandlerXmlrpcP->xmlProcessor = parmsP->xml_processor;
        else
            xmlrpc_faultf(envP, "Parameter too short to contain the required "
                          "'xml_processor' member");
    }
    if (!envP->fault_occurred) {
        if (parmSize >= XMLRPC_AHPSIZE(xml_processor_arg))
            uriHandlerXmlrpcP->xmlProcessorArg = parmsP->xml_processor_arg;
        else
            xmlrpc_faultf(envP, "Parameter too short to contain the required "
                          "'xml_processor_arg' member");
    }
    if (!envP->fault_occurred) {
        if (parmSize >= XMLRPC_AHPSIZE(xml_processor_max_stack))
            xmlProcessorMaxStackSize = parmsP->xml_processor_max_stack;
        else
            xmlrpc_faultf(envP, "Parameter too short to contain the required "
                          "'xml_processor_max_stack' member");
    }
    if (!envP->fault_occurred) {
        if (parmSize >= XMLRPC_AHPSIZE(uri_path) && parmsP->uri_path)
            uriHandlerXmlrpcP->uriPath = xmlrpc_strdupsol(parmsP->uri_path);
        else
            uriHandlerXmlrpcP->uriPath = xmlrpc_strdupsol("/RPC2");

        if (parmSize >= XMLRPC_AHPSIZE(chunk_response) &&
            parmsP->chunk_response)
            uriHandlerXmlrpcP->chunkResponse = parmsP->chunk_response;
        else
            uriHandlerXmlrpcP->chunkResponse = false;
        
        interpretHttpAccessControl(parmsP, parmSize,
                                   &uriHandlerXmlrpcP->accessControl);

        if (envP->fault_occurred)
            xmlrpc_termAccessControl(&uriHandlerXmlrpcP->accessControl);
    }
    if (!envP->fault_occurred)
        setHandler(envP, srvP, uriHandlerXmlrpcP, xmlProcessorMaxStackSize);

    if (envP->fault_occurred)
        free(uriHandlerXmlrpcP);
}



void
xmlrpc_server_abyss_set_handler2(
    TServer *         const srvP,
    const char *      const uriPath,
    xmlrpc_call_processor   xmlProcessor,
    void *            const xmlProcessorArg,
    size_t            const xmlProcessorMaxStackSize,
    xmlrpc_bool       const chunkResponse) {

    xmlrpc_env env;
    xmlrpc_server_abyss_handler_parms parms;

    xmlrpc_env_init(&env);

    parms.xml_processor = xmlProcessor;
    parms.xml_processor_arg = xmlProcessorArg;
    parms.xml_processor_max_stack = xmlProcessorMaxStackSize;
    parms.uri_path = uriPath;
    parms.chunk_response = chunkResponse;

    xmlrpc_server_abyss_set_handler3(&env, srvP,
                                     &parms, XMLRPC_AHPSIZE(chunk_response));
    
    if (env.fault_occurred)
        abort();

    xmlrpc_env_clean(&env);
}



void
xmlrpc_server_abyss_set_handler(xmlrpc_env *      const envP,
                                TServer *         const srvP,
                                const char *      const uriPath,
                                xmlrpc_registry * const registryP) {

    xmlrpc_server_abyss_handler_parms parms;

    parms.xml_processor = &processXmlrpcCall;
    parms.xml_processor_arg = registryP;
    parms.xml_processor_max_stack = xmlrpc_registry_max_stackSize(registryP);
    parms.uri_path = uriPath;

    xmlrpc_server_abyss_set_handler3(envP, srvP,
                                     &parms, XMLRPC_AHPSIZE(uri_path));
}

    

void
xmlrpc_server_abyss_set_default_handler(TServer * const srvP) {

    ServerDefaultHandler(srvP, xmlrpc_serverAbyssDefaultUriHandler);
}
    


static void
setHandlersRegistry(TServer *         const srvP,
                    const char *      const uriPath,
                    xmlrpc_registry * const registryP,
                    bool              const chunkResponse,
                    const char *      const allowOrigin,
                    bool              const expires,
                    unsigned int      const maxAge) {

    xmlrpc_env env;
    xmlrpc_server_abyss_handler_parms parms;

    xmlrpc_env_init(&env);

    parms.xml_processor = &processXmlrpcCall;
    parms.xml_processor_arg = registryP;
    parms.xml_processor_max_stack = xmlrpc_registry_max_stackSize(registryP),
    parms.uri_path = uriPath;
    parms.chunk_response = chunkResponse;
    parms.allow_origin = allowOrigin;
    parms.access_ctl_expires = expires;
    parms.access_ctl_max_age = maxAge;

    xmlrpc_server_abyss_set_handler3(
        &env, srvP, &parms, XMLRPC_AHPSIZE(access_ctl_max_age));
    
    if (env.fault_occurred)
        abort();

    xmlrpc_env_clean(&env);

    xmlrpc_server_abyss_set_default_handler(srvP);
}



void
xmlrpc_server_abyss_set_handlers2(TServer *         const srvP,
                                  const char *      const uriPath,
                                  xmlrpc_registry * const registryP) {

    setHandlersRegistry(srvP, uriPath, registryP, false, NULL, false, 0);
}



void
xmlrpc_server_abyss_set_handlers(TServer *         const srvP,
                                 xmlrpc_registry * const registryP) {

    setHandlersRegistry(srvP, "/RPC2", registryP, false, NULL, false, 0);
}



/*============================================================================
  createServer()
============================================================================*/

static void
setAdditionalServerParms(const xmlrpc_server_abyss_parms * const parmsP,
                         unsigned int                      const parmSize,
                         TServer *                         const serverP) {

    if (parmSize >= XMLRPC_APSIZE(keepalive_timeout) &&
        parmsP->keepalive_timeout > 0)
        ServerSetKeepaliveTimeout(serverP, parmsP->keepalive_timeout);
    if (parmSize >= XMLRPC_APSIZE(keepalive_max_conn) &&
        parmsP->keepalive_max_conn > 0)
        ServerSetKeepaliveMaxConn(serverP, parmsP->keepalive_max_conn);
    if (parmSize >= XMLRPC_APSIZE(timeout) &&
        parmsP->timeout > 0)
        ServerSetTimeout(serverP, parmsP->timeout);
    if (parmSize >= XMLRPC_APSIZE(dont_advertise))
        ServerSetAdvertise(serverP, !parmsP->dont_advertise);
    if (parmSize >= XMLRPC_APSIZE(max_conn)) {
        if (parmsP->max_conn != 0)
            ServerSetMaxConn(serverP, parmsP->max_conn);
    }
    if (parmSize >= XMLRPC_APSIZE(max_conn_backlog)) {
        if (parmsP->max_conn_backlog != 0)
            ServerSetMaxConnBacklog(serverP, parmsP->max_conn_backlog);
    }
}



static void
extractSockAddrParms(xmlrpc_env *                      const envP,
                     const xmlrpc_server_abyss_parms * const parmsP,
                     unsigned int                      const parmSize,
                     const struct sockaddr **          const sockAddrPP,
                     socklen_t *                       const sockAddrLenP,
                     unsigned int *                    const portNumberP) {
/*----------------------------------------------------------------------------
   Return the server parameters that affect the address on which the server
   socket shall listen.

   There are two ways the arguments can specify this: 1) user supplies a
   complete socket address, which specifies both a TCP port number and an IP
   address (which determines on which interface, ergo which network, if any,
   the server listens); and 2) just a TCP port number, which means he wants
   to listen on all IPv4 interfaces and networks.  (2) is legacy.

   If the user specifies the 'sockaddrP' and 'sockaddrlen' arguments, he gets
   (1) and we ignore his 'port' argument.  We return his 'sockaddrP' and
   'sockaddrlen' values as *sockAddrPP and *sockAddrLenP and nothing as
   *portNumberP.

   If the user doesn't specify 'sockaddrP', he gets (2).  We return NULL as
   *sockAddrP and his 'port_number' argument as *portNumberP.  If he doesn't
   specify 'port' either, we default it to 8080.

   Specifying 'sockaddrP' and not 'sockaddrlen' is an error.

   Note that the user's socket address may indicate "any IP address."
-----------------------------------------------------------------------------*/
    if (parmSize >= XMLRPC_APSIZE(sockaddr_p)) {
        if (parmSize < XMLRPC_APSIZE(sockaddrlen))
            xmlrpc_faultf(envP, "You must specify 'sockaddrlen' when you "
                          "specify 'sockaddrP'");
        else {
            *sockAddrPP   = parmsP->sockaddr_p;
            *sockAddrLenP = parmsP->sockaddrlen;
        }
    } else
        *sockAddrPP = NULL;

    if (*sockAddrPP == NULL) {
        unsigned int portNumber;

        if (parmSize >= XMLRPC_APSIZE(port_number))
            portNumber = parmsP->port_number;
        else
            portNumber = 8080;
        
        if (portNumber > 0xffff)
            xmlrpc_faultf(envP,
                          "TCP port number %u exceeds the maximum possible "
                          "TCP port number (65535)",
                          portNumber);

        *portNumberP = portNumber;
    }        
}



static void
extractServerCreateParms(
    xmlrpc_env *                      const envP,
    const xmlrpc_server_abyss_parms * const parmsP,
    unsigned int                      const parmSize,
    bool *                            const socketBoundP,
    const struct sockaddr **          const sockAddrPP,
    socklen_t *                       const sockAddrLenP,
    unsigned int *                    const portNumberP,
    TOsSocket *                       const socketFdP,
    const char **                     const logFileNameP) {
                   

    if (parmSize >= XMLRPC_APSIZE(socket_bound))
        *socketBoundP = parmsP->socket_bound;
    else
        *socketBoundP = FALSE;

    if (*socketBoundP) {
        if (parmSize < XMLRPC_APSIZE(socket_handle))
            xmlrpc_faultf(envP, "socket_bound is true, but server parameter "
                          "structure does not contain socket_handle (it's too "
                          "short)");
        else
            *socketFdP = parmsP->socket_handle;
    } else {
        extractSockAddrParms(envP, parmsP, parmSize, sockAddrPP, sockAddrLenP,
                             portNumberP);
    }
    if (!envP->fault_occurred) {
        if (parmSize >= XMLRPC_APSIZE(log_file_name) &&
            parmsP->log_file_name)
            *logFileNameP = strdup(parmsP->log_file_name);
        else
            *logFileNameP = NULL;
    }
}



static void
chanSwitchCreateOsSocket(TOsSocket      const socketFd,
                         TChanSwitch ** const chanSwitchPP,
                         const char **  const errorP) {

#ifdef _WIN32
    ChanSwitchWinCreateWinsock(socketFd, chanSwitchPP, errorP);
#else
    ChanSwitchUnixCreateFd(socketFd, chanSwitchPP, errorP);
#endif

}



static void
createChanSwitchOsSocket(xmlrpc_env *   const envP,
                         TOsSocket      const socketFd,
                         TChanSwitch ** const chanSwitchPP) {

    const char * error;

    chanSwitchCreateOsSocket(socketFd, chanSwitchPP, &error);

    if (error) {
        xmlrpc_faultf(envP, "Unable to create Abyss channel switch out of "
                      "file descriptor %d.  %s", socketFd, error);
        xmlrpc_strfree(error);
    }
}



static void
chanSwitchCreateSockAddr(int                     const protocolFamily,
                         const struct sockaddr * const sockAddrP,
                         socklen_t               const sockAddrLen,
                         TChanSwitch **          const chanSwitchPP,
                         const char **           const errorP) {

#ifdef _WIN32
    ChanSwitchWinCreate2(protocolFamily, sockAddrP, sockAddrLen, 
                          chanSwitchPP, errorP);
#else
    ChanSwitchUnixCreate2(protocolFamily, sockAddrP, sockAddrLen, 
                          chanSwitchPP, errorP);
#endif

}



static void
createChanSwitchSockAddr(xmlrpc_env *            const envP,
                         const struct sockaddr * const sockAddrP,
                         socklen_t               const sockAddrLen,
                         TChanSwitch **          const chanSwitchPP) {

    int protocolFamily;

    assert(sockAddrP);

    switch (sockAddrP->sa_family) {
    case AF_INET:
        protocolFamily = PF_INET;
        break;
    case AF_INET6:
        protocolFamily = PF_INET6;
        break;
    default:
        xmlrpc_faultf(envP, "Unknown socket address family %d.  "
                      "We know only AF_INET and AF_INET6.",
                      sockAddrP->sa_family);
    }

    if (!envP->fault_occurred) {
        const char * error;

        chanSwitchCreateSockAddr(protocolFamily, sockAddrP, sockAddrLen,
                                 chanSwitchPP, &error);

        if (error) {
            xmlrpc_faultf(envP, "Unable to create Abyss channel switch "
                          "given the socket address.  %s", error);
            xmlrpc_strfree(error);
        }
    }
}



static void
createChanSwitchIpv4Port(xmlrpc_env *          const envP,
                         unsigned int          const portNumber,
                         TChanSwitch **        const chanSwitchPP) {

    struct sockaddr_in sockAddr;
    const char * error;

    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_port        = htons(portNumber);
    sockAddr.sin_addr.s_addr = INADDR_ANY;

    chanSwitchCreateSockAddr(PF_INET, (const struct sockaddr *)&sockAddr,
                             sizeof(sockAddr),
                             chanSwitchPP, &error);
    
    if (error) {
        xmlrpc_faultf(envP, "Unable to create Abyss channel switch "
                      "to listen on Port %u at any IPv4 address.  %s",
                      portNumber, error);
        xmlrpc_strfree(error);
    }
}



static void
createServerBare(xmlrpc_env *                      const envP,
                 const xmlrpc_server_abyss_parms * const parmsP,
                 unsigned int                      const parmSize,
                 TServer *                         const serverP,
                 TChanSwitch **                    const chanSwitchPP) {
/*----------------------------------------------------------------------------
   Create a bare server.  It will need further setup before it is ready
   to use.
-----------------------------------------------------------------------------*/
    bool socketBound;
    const struct sockaddr * sockAddrP;
    socklen_t sockAddrLen;
    unsigned int portNumber;
    TOsSocket socketFd;
    const char * logFileName;

    extractServerCreateParms(envP, parmsP, parmSize,
                             &socketBound,
                             &sockAddrP, &sockAddrLen, &portNumber, &socketFd,
                             &logFileName);

    if (!envP->fault_occurred) {
        TChanSwitch * chanSwitchP;

        if (socketBound)
            createChanSwitchOsSocket(envP, socketFd, &chanSwitchP);
        else {
            if (sockAddrP)
                createChanSwitchSockAddr(envP, sockAddrP, sockAddrLen,
                                         &chanSwitchP);
            else
                createChanSwitchIpv4Port(envP, portNumber, &chanSwitchP);
        }
        if (!envP->fault_occurred) {
            const char * error;

            ServerCreateSwitch(serverP, chanSwitchP, &error);

            if (error) {
                xmlrpc_faultf(envP, "Abyss failed to create server.  %s",
                              error);
                xmlrpc_strfree(error);
            } else {
                *chanSwitchPP = chanSwitchP;
                    
                ServerSetName(serverP, "XmlRpcServer");
            
                if (logFileName)
                    ServerSetLogFileName(serverP, logFileName);
            }
            if (envP->fault_occurred)
                ChanSwitchDestroy(chanSwitchP);
        }
        if (logFileName)
            xmlrpc_strfree(logFileName);
    }
}



static const char *
uriPathParm(const xmlrpc_server_abyss_parms * const parmsP,
            unsigned int                      const parmSize) {
    
    const char * uriPath;

    if (parmSize >= XMLRPC_APSIZE(uri_path) && parmsP->uri_path)
        uriPath = parmsP->uri_path;
    else
        uriPath = "/RPC2";

    return uriPath;
}



static bool
chunkResponseParm(const xmlrpc_server_abyss_parms * const parmsP,
                  unsigned int                      const parmSize) {

    return
        parmSize >= XMLRPC_APSIZE(chunk_response) &&
        parmsP->chunk_response;
}    



static const char *
allowOriginParm(const xmlrpc_server_abyss_parms * const parmsP,
                unsigned int                      const parmSize) {

    return
        parmSize >= XMLRPC_APSIZE(allow_origin) ?
        parmsP->allow_origin : NULL;
}    



static bool
expiresParm(const xmlrpc_server_abyss_parms * const parmsP,
            unsigned int                      const parmSize) {

    return
        parmSize >= XMLRPC_APSIZE(access_ctl_expires) ?
        parmsP->access_ctl_expires : false;
}    



static unsigned int
maxAgeParm(const xmlrpc_server_abyss_parms * const parmsP,
           unsigned int                      const parmSize) {

    return
        parmSize >= XMLRPC_APSIZE(access_ctl_max_age) ?
        parmsP->access_ctl_max_age : 0;
}    



static void
createServer(xmlrpc_env *                      const envP,
             const xmlrpc_server_abyss_parms * const parmsP,
             unsigned int                      const parmSize,
             TServer *                         const abyssServerP,
             TChanSwitch **                    const chanSwitchPP) {

    createServerBare(envP, parmsP, parmSize, abyssServerP, chanSwitchPP);

    if (!envP->fault_occurred) {
        const char * error;

        setAdditionalServerParms(parmsP, parmSize, abyssServerP);
        
        setHandlersRegistry(abyssServerP, uriPathParm(parmsP, parmSize),
                            parmsP->registryP,
                            chunkResponseParm(parmsP, parmSize),
                            allowOriginParm(parmsP, parmSize),
                            expiresParm(parmsP, parmSize),
                            maxAgeParm(parmsP, parmSize));
        
        ServerInit2(abyssServerP, &error);

        if (error) {
            xmlrpc_faultf(envP, error);
            xmlrpc_strfree(error);
        }
    }
}



static bool
enableShutdownParm(const xmlrpc_server_abyss_parms * const parmsP,
                   unsigned int                      const parmSize) {

    return
        parmSize >= XMLRPC_APSIZE(enable_shutdown) &&
        parmsP->enable_shutdown;
}



static xmlrpc_server_shutdown_fn shutdownAbyss;

static void
shutdownAbyss(xmlrpc_env * const faultP,
              void *       const context,
              const char * const comment ATTR_UNUSED,
              void *       const callInfo ATTR_UNUSED) {
/*----------------------------------------------------------------------------
   Tell Abyss to wrap up whatever it's doing and shut down.

   This is a server shutdown function to be registered in the method
   registry, for use by the 'system.shutdown' system method.

   After we return, Abyss will finish up the system.shutdown and any
   other connections that are in progress, then the call to
   ServerRun() etc. will return.

   *faultP is the result of the shutdown request, not whether we
   succeeded or failed.  We are not allowed to fail.
-----------------------------------------------------------------------------*/
    xmlrpc_server_abyss_t * const serverP = context;

    xmlrpc_env_init(faultP);
    
    if (!serverP->shutdownEnabled)
        xmlrpc_env_set_fault_formatted(
            faultP, XMLRPC_REQUEST_REFUSED_ERROR,
            "Shutdown by client is disabled on this server.");
    else
        ServerTerminate(&serverP->abyssServer);
}



/*=============================================================================
  xmlrpc_server_abyss object methods
=============================================================================*/

void
xmlrpc_server_abyss_create(xmlrpc_env *                      const envP,
                           const xmlrpc_server_abyss_parms * const parmsP,
                           unsigned int                      const parmSize,
                           xmlrpc_server_abyss_t **          const serverPP) {

    xmlrpc_server_abyss_t * serverP;

    XMLRPC_ASSERT_ENV_OK(envP);

    validateGlobalInit(envP);

    if (!envP->fault_occurred) {
        if (parmSize < XMLRPC_APSIZE(registryP))
            xmlrpc_faultf(envP,
                          "You must specify members at least up through "
                          "'registryP' in the server parameters argument.  "
                          "That would mean the parameter size would be >= %u "
                          "but you specified a size of %u",
                          (unsigned)XMLRPC_APSIZE(registryP), parmSize);
        else {
            MALLOCVAR(serverP);

            if (serverP == NULL)
                xmlrpc_faultf(envP, "Unable to allocate memory for "
                              "server descriptor.");
            else {
                createServer(envP, parmsP, parmSize,
                             &serverP->abyssServer, &serverP->chanSwitchP);
            
                if (!envP->fault_occurred) {
                    serverP->shutdownEnabled =
                        enableShutdownParm(parmsP, parmSize);

                    xmlrpc_registry_set_shutdown(
                        parmsP->registryP, &shutdownAbyss, serverP);
                
                    if (envP->fault_occurred)
                        free(serverP);
                    else
                        *serverPP = serverP;
                }
            }
        }
    }
}



void
xmlrpc_server_abyss_destroy(xmlrpc_server_abyss_t * const serverP) {

    XMLRPC_ASSERT(globallyInitialized);

    ServerFree(&serverP->abyssServer);

    if (serverP->chanSwitchP)
        ChanSwitchDestroy(serverP->chanSwitchP);

    free(serverP);
}



void
xmlrpc_server_abyss_use_sigchld(xmlrpc_server_abyss_t * const serverP) {

    ServerUseSigchld(&serverP->abyssServer);
}



void
xmlrpc_server_abyss_run_server(xmlrpc_env *            const envP ATTR_UNUSED,
                               xmlrpc_server_abyss_t * const serverP) {

    ServerRun(&serverP->abyssServer);
}



void
xmlrpc_server_abyss_terminate(
    xmlrpc_env *            const envP ATTR_UNUSED,
    xmlrpc_server_abyss_t * const serverP) {

    ServerTerminate(&serverP->abyssServer);
}



void
xmlrpc_server_abyss_reset_terminate(
    xmlrpc_env *            const envP ATTR_UNUSED,
    xmlrpc_server_abyss_t * const serverP) {

    ServerResetTerminate(&serverP->abyssServer);
}



static void 
sigchld(int const signalClass ATTR_UNUSED) {
/*----------------------------------------------------------------------------
   This is a signal handler for a SIGCHLD signal (which informs us that
   one of our child processes has terminated).

   The only child processes we have are those that belong to the Abyss
   server (and then only if the Abyss server was configured to use
   forking as a threading mechanism), so we respond by passing the
   signal on to the Abyss server.  And reaping the dead child.
-----------------------------------------------------------------------------*/
#ifndef _WIN32
    /* Reap zombie children / report to Abyss until there aren't any more. */

    bool childrenLeft;
    bool error;

    assert(signalClass == SIGCHLD);

    error = false;
    childrenLeft = true;  /* initial assumption */
    
    /* Reap defunct children until there aren't any more. */
    while (childrenLeft && !error) {
        int status;
        pid_t pid;

        pid = waitpid((pid_t) -1, &status, WNOHANG);
    
        if (pid == 0)
            childrenLeft = false;
        else if (pid < 0) {
            /* because of ptrace */
            if (errno != EINTR)   
                error = true;
        } else
            ServerHandleSigchld(pid);
    }
#endif /* _WIN32 */
}


struct xmlrpc_server_abyss_sig {

    /* A description of the state of the process' signal handlers before
       functions in this library messed with them; useful for restoring
       them later.
    */
#ifndef _WIN32
    struct sigaction pipe;
    struct sigaction chld;
#else
    int dummy;
#endif
};



static void
setupSignalHandlers(xmlrpc_server_abyss_sig * const oldHandlersP) {
#ifndef _WIN32
    struct sigaction mysigaction;
    
    sigemptyset(&mysigaction.sa_mask);
    mysigaction.sa_flags = 0;

    /* This signal indicates connection closed in the middle */
    mysigaction.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &mysigaction, &oldHandlersP->pipe);
    
    /* This signal indicates a child process (request handler) has died */
    mysigaction.sa_handler = sigchld;
    sigaction(SIGCHLD, &mysigaction, &oldHandlersP->chld);
#endif
}    



static void
restoreSignalHandlers(const xmlrpc_server_abyss_sig * const oldHandlersP) {
#ifndef _WIN32

    sigaction(SIGPIPE, &oldHandlersP->pipe, NULL);
    sigaction(SIGCHLD, &oldHandlersP->chld, NULL);

#endif
}



void
xmlrpc_server_abyss_setup_sig(
    xmlrpc_env *               const envP,
    xmlrpc_server_abyss_t *    const serverP,
    xmlrpc_server_abyss_sig ** const oldHandlersPP) {

    xmlrpc_server_abyss_sig * oldHandlersP;

    XMLRPC_ASSERT_ENV_OK(envP);

    validateGlobalInit(envP);

    if (!envP->fault_occurred) {
        MALLOCVAR(oldHandlersP);

        if (oldHandlersP == NULL)
            xmlrpc_faultf(envP, "Unable to allocate memory to save signal "
                          "handling state.");
        else {
            setupSignalHandlers(oldHandlersP);

            xmlrpc_server_abyss_use_sigchld(serverP);
        }
        if (oldHandlersPP)
            *oldHandlersPP = oldHandlersP;
        else
            free(oldHandlersP);
    }
}



void
xmlrpc_server_abyss_restore_sig(
    const xmlrpc_server_abyss_sig * const oldHandlersP) {

    restoreSignalHandlers(oldHandlersP);
}
                                 


static void
runServerDaemon(TServer *  const serverP,
                runfirstFn const runfirst,
                void *     const runfirstArg) {

    xmlrpc_server_abyss_sig oldHandlers;

    setupSignalHandlers(&oldHandlers);

    ServerUseSigchld(serverP);

    ServerDaemonize(serverP);
    
    /* We run the user supplied runfirst after forking, but before accepting
       connections (helpful when running with threads)
    */
    if (runfirst)
        runfirst(runfirstArg);

    ServerRun(serverP);

    restoreSignalHandlers(&oldHandlers);
}



static void
oldHighLevelAbyssRun(xmlrpc_env *                      const envP,
                     const xmlrpc_server_abyss_parms * const parmsP,
                     unsigned int                      const parmSize) {
/*----------------------------------------------------------------------------
   This is the old deprecated interface, where the caller of the 
   xmlrpc_server_abyss API supplies an Abyss configuration file and
   we use it to daemonize (fork into the background, chdir, set uid, etc.)
   and run the Abyss server.

   The new preferred interface, implemented by normalLevelAbyssRun(),
   instead lets Caller set up the process environment himself and pass
   Abyss parameters in memory.  That's a more conventional and
   flexible API.
-----------------------------------------------------------------------------*/
    TServer server;
    abyss_bool success;

    success = ServerCreate(&server, "XmlRpcServer", 8080, DEFAULT_DOCS, NULL);

    if (!success)
        xmlrpc_faultf(envP, "Failed to create Abyss server object");
    else {
        runfirstFn runfirst;
        void * runfirstArg;

        assert(parmSize >= XMLRPC_APSIZE(config_file_name));
    
        ConfReadServerFile(parmsP->config_file_name, &server);
        
        assert(parmSize >= XMLRPC_APSIZE(registryP));
    
        setHandlersRegistry(&server, "/RPC2", parmsP->registryP, false, NULL,
                            false, 0);
        
        ServerInit(&server);
    
        if (parmSize >= XMLRPC_APSIZE(runfirst_arg)) {
            runfirst    = parmsP->runfirst;
            runfirstArg = parmsP->runfirst_arg;
        } else {
            runfirst    = NULL;
            runfirstArg = NULL;
        }
        runServerDaemon(&server, runfirst, runfirstArg);

        ServerFree(&server);
    }
}



static void
normalLevelAbyssRun(xmlrpc_env *                      const envP,
                    const xmlrpc_server_abyss_parms * const parmsP,
                    unsigned int                      const parmSize) {
    
    xmlrpc_server_abyss_t * serverP;

    xmlrpc_server_abyss_create(envP, parmsP, parmSize, &serverP);

    if (!envP->fault_occurred) {
        xmlrpc_server_abyss_sig * oldHandlersP;

        xmlrpc_server_abyss_setup_sig(envP, serverP, &oldHandlersP);

        if (!envP->fault_occurred) {
            xmlrpc_server_abyss_run_server(envP, serverP);

            xmlrpc_server_abyss_restore_sig(oldHandlersP);

            free(oldHandlersP);
        }
        xmlrpc_server_abyss_destroy(serverP);
    }
}



void
xmlrpc_server_abyss(xmlrpc_env *                      const envP,
                    const xmlrpc_server_abyss_parms * const parmsP,
                    unsigned int                      const parmSize) {
/*----------------------------------------------------------------------------
   Note that this is not re-entrant and not thread-safe, due to the
   global library initialization.  If you want to run a server inside
   a thread of a multi-threaded program, use
   xmlrpc_server_abyss_create() instead.  As required by that
   subroutine, your program will contain a call to
   xmlrpc_server_abyss_global_init() early in your program, when it is only
   one thread.
-----------------------------------------------------------------------------*/
    XMLRPC_ASSERT_ENV_OK(envP);

    xmlrpc_server_abyss_global_init(envP);

    if (!envP->fault_occurred) {
        if (parmSize < XMLRPC_APSIZE(registryP))
            xmlrpc_faultf(envP,
                          "You must specify members at least up through "
                          "'registryP' in the server parameters argument.  "
                          "That would mean the parameter size would be >= %u "
                          "but you specified a size of %u",
                          (unsigned)XMLRPC_APSIZE(registryP), parmSize);
        else {
            if (parmsP->config_file_name)
                oldHighLevelAbyssRun(envP, parmsP, parmSize);
            else
                normalLevelAbyssRun(envP, parmsP, parmSize);
        }
        xmlrpc_server_abyss_global_term();
    }
}



/*=========================================================================
  XML-RPC Server Method Registry

  This is an old deprecated form of the server facilities that uses
  global variables.
=========================================================================*/

/* These global variables must be treated as read-only after the
   server has started.
*/

static TServer globalSrv;
    /* When you use the old interface (xmlrpc_server_abyss_init(), etc.),
       this is the Abyss server to which they refer.  Obviously, there can be
       only one Abyss server per program using this interface.
    */

static xmlrpc_registry * builtin_registryP;



void 
xmlrpc_server_abyss_init_registry(void) {

    /* This used to just create the registry and Caller would be
       responsible for adding the handlers that use it.

       But that isn't very modular -- the handlers and registry go
       together; there's no sense in using the built-in registry and
       not the built-in handlers because if you're custom building
       something, you can just make your own regular registry.  So now
       we tie them together, and we don't export our handlers.  
    */
    xmlrpc_env env;

    xmlrpc_env_init(&env);
    builtin_registryP = xmlrpc_registry_new(&env);
    dieIfFaultOccurred(&env);
    xmlrpc_env_clean(&env);

    setHandlersRegistry(&globalSrv, "/RPC2", builtin_registryP, false, NULL,
                        false, 0);
}



xmlrpc_registry *
xmlrpc_server_abyss_registry(void) {

    /* This is highly deprecated.  If you want to mess with a registry,
       make your own with xmlrpc_registry_new() -- don't mess with the
       internal one.
    */
    return builtin_registryP;
}



/* A quick & easy shorthand for adding a method. */
void 
xmlrpc_server_abyss_add_method(char *        const method_name,
                               xmlrpc_method const method,
                               void *        const user_data) {
    xmlrpc_env env;

    xmlrpc_env_init(&env);
    xmlrpc_registry_add_method(&env, builtin_registryP, NULL, method_name,
                               method, user_data);
    dieIfFaultOccurred(&env);
    xmlrpc_env_clean(&env);
}



void
xmlrpc_server_abyss_add_method_w_doc(char *        const method_name,
                                     xmlrpc_method const method,
                                     void *        const user_data,
                                     char *        const signature,
                                     char *        const help) {

    xmlrpc_env env;
    xmlrpc_env_init(&env);
    xmlrpc_registry_add_method_w_doc(
        &env, builtin_registryP, NULL, method_name,
        method, user_data, signature, help);
    dieIfFaultOccurred(&env);
    xmlrpc_env_clean(&env);    
}



void 
xmlrpc_server_abyss_init(int          const flags ATTR_UNUSED, 
                         const char * const config_file) {

    abyss_bool success;

    success = ServerCreate(&globalSrv, "XmlRpcServer", 8080,
                           DEFAULT_DOCS, NULL);

    if (!success)
        abort();
    else {
        ConfReadServerFile(config_file, &globalSrv);

        xmlrpc_server_abyss_init_registry();
            /* Installs /RPC2 handler and default handler that use the
               built-in registry.
            */

        ServerInit(&globalSrv);
    }
}



void 
xmlrpc_server_abyss_run_first(runfirstFn const runfirst,
                              void *     const runfirstArg) {
    
    runServerDaemon(&globalSrv, runfirst, runfirstArg);
}



void 
xmlrpc_server_abyss_run(void) {
    runServerDaemon(&globalSrv, NULL, NULL);
}



/*
** Copyright (C) 2001 by First Peer, Inc. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission. 
**  
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
** There is more copyright information in the bottom half of this file. 
** Please see it for more details. 
*/
