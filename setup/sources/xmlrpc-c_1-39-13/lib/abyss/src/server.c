/* Copyright information is at end of file */

#define _XOPEN_SOURCE 600  /* Make sure strdup() is in <string.h> */
#define _BSD_SOURCE  /* Make sure setgroups()is in <grp.h> */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#ifndef _WIN32
  #include <grp.h>
#endif

#include "xmlrpc_config.h"
#include "bool.h"
#include "girmath.h"
#include "mallocvar.h"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/sleep_int.h"
#include "xmlrpc-c/lock.h"
#include "xmlrpc-c/lock_platform.h"

#include "xmlrpc-c/abyss.h"
#include "trace.h"
#include "thread.h"
#include "session.h"
#include "file.h"
#include "conn.h"
#include "chanswitch.h"
#include "channel.h"
#include "socket.h"
#ifdef _WIN32
  #include "socket_win.h"
#else
  #include "socket_unix.h"
#endif
#include "http.h"
#include "handler.h"

#include "server.h"


struct uriHandler {
    initHandlerFn init;
    termHandlerFn term;
    handleReq3Fn  handleReq3;
    handleReq2Fn  handleReq2;
    URIHandler    handleReq1;
    void *        userdata;
};



void
ServerTerminate(TServer * const serverP) {

    struct _TServer * const srvP = serverP->srvP;

    srvP->terminationRequested = true;

    if (srvP->chanSwitchP)
        ChanSwitchInterrupt(srvP->chanSwitchP);
}



void
ServerResetTerminate(TServer * const serverP) {

    struct _TServer * const srvP = serverP->srvP;

    srvP->terminationRequested = false;
}



static void
initUnixStuff(struct _TServer * const srvP) {
#ifndef _WIN32
    srvP->pidfileP = NULL;
    srvP->uid = srvP->gid = -1;
#endif
}



static void
logOpen(struct _TServer * const srvP,
        const char **     const errorP) {

    bool success;

    success = FileOpenCreate(&srvP->logfileP, srvP->logfilename,
                             O_WRONLY | O_APPEND);
    if (success) {
        srvP->logLockP = xmlrpc_lock_create();
        *errorP = NULL;

        srvP->logfileisopen = true;
            
        if (*errorP)
            FileClose(srvP->logfileP);
    } else
        xmlrpc_asprintf(errorP, "Can't open log file '%s'", srvP->logfilename);
}



static void
logClose(struct _TServer * const srvP) {

    if (srvP->logfileisopen) {
        FileClose(srvP->logfileP);
        srvP->logLockP->destroy(srvP->logLockP);
        srvP->logfileisopen = false;
    }
}



static void
setupTrace(struct _TServer * const srvP) {

    srvP->traceIsActive = (getenv("ABYSS_TRACE_SERVER") != NULL);

    if (srvP->traceIsActive)
        fprintf(stderr, "Abyss server will trace "
                "basic server activity "
                "because of ABYSS_TRACE_SERVER environment variable\n");
}



static void
tracev(const char * const fmt,
       va_list            argptr) {

    vfprintf(stderr, fmt, argptr);

    fprintf(stderr, "\n");
}



static void
trace(struct _TServer * const srvP,
      const char *      const fmt,
      ...) {

    if (srvP->traceIsActive) {
        va_list argptr;

        va_start(argptr, fmt);
        tracev(fmt, argptr);
        va_end(argptr);
    }
}



static void
initChanSwitchStuff(struct _TServer * const srvP,
                    bool              const noAccept,
                    TChanSwitch *     const chanSwitchP,
                    bool              const userChanSwitch,
                    unsigned short    const port,
                    const char **     const errorP) {
    

    if (chanSwitchP) {
        *errorP = NULL;
        srvP->serverAcceptsConnections = true;
        srvP->chanSwitchP = chanSwitchP;
        srvP->weCreatedChanSwitch = !userChanSwitch;
    } else if (noAccept) {
        *errorP = NULL;
        srvP->serverAcceptsConnections = false;
        srvP->chanSwitchP = NULL;
        srvP->weCreatedChanSwitch = false;
    } else {
        *errorP = NULL;
        srvP->serverAcceptsConnections = true;
        srvP->chanSwitchP = NULL;
        srvP->weCreatedChanSwitch = false;
        srvP->port = port;
    }
    srvP->readyToAccept = false;
}



static void
createServer(struct _TServer ** const srvPP,
             bool               const noAccept,
             TChanSwitch *      const chanSwitchP,
             bool               const userChanSwitch,
             unsigned short     const portNumber,             
             const char **      const errorP) {

    struct _TServer * srvP;

    MALLOCVAR(srvP);

    if (srvP == NULL) {
        xmlrpc_asprintf(errorP,
                        "Unable to allocate space for server descriptor");
    } else {
        setupTrace(srvP);

        srvP->terminationRequested = false;

        initChanSwitchStuff(srvP, noAccept, chanSwitchP, userChanSwitch,
                            portNumber, errorP);

        if (!*errorP) {
            srvP->builtinHandlerP = HandlerCreate();
            if (!srvP->builtinHandlerP)
                xmlrpc_asprintf(errorP, "Unable to allocate space for "
                                "builtin handler descriptor");
            else {
                srvP->defaultHandler   = HandlerDefaultBuiltin;
                srvP->defaultHandlerContext = srvP->builtinHandlerP;

                srvP->name             = strdup("unnamed");
                srvP->logfilename      = NULL;
                srvP->keepalivetimeout = 15;
                srvP->keepalivemaxconn = 30;
                srvP->timeout          = 15;
                srvP->advertise        = true;
                srvP->useSigchld       = false;
                srvP->uriHandlerStackSize = 0;
                srvP->maxConn          = 15;
                srvP->maxConnBacklog   = 15;
            
                initUnixStuff(srvP);

                ListInitAutoFree(&srvP->handlers);

                srvP->logfileisopen = false;
                
                *errorP = NULL;

                if (*errorP)
                    HandlerDestroy(srvP->builtinHandlerP);
            }
        }        
        if (*errorP)
            free(srvP);
    }
    *srvPP = srvP;
}



static void
setNamePathLog(TServer *    const serverP,
               const char * const name,
               const char * const filesPath,
               const char * const logFileName) {
/*----------------------------------------------------------------------------
   This odd function exists to help with backward compatibility.
   Today, we have the expandable model where you create a server with
   default parameters, then use ServerSet... functions to choose
   non-default parameters.  But before, you specified these three
   parameters right in the arguments of various create functions.
-----------------------------------------------------------------------------*/
    if (name)
        ServerSetName(serverP, name);
    if (filesPath)
        ServerSetFilesPath(serverP, filesPath);
    if (logFileName)
        ServerSetLogFileName(serverP, logFileName);
}



abyss_bool
ServerCreate(TServer *       const serverP,
             const char *    const name,
             xmlrpc_uint16_t const portNumber,
             const char *    const filesPath,
             const char *    const logFileName) {

    bool const noAcceptFalse = false;
    bool const userChanSwitchFalse = false;

    bool success;
    const char * error;

    createServer(&serverP->srvP, noAcceptFalse,
                 NULL, userChanSwitchFalse,
                 portNumber, &error);

    if (error) {
        TraceMsg(error);
        xmlrpc_strfree(error);
        success = false;
    } else {
        success = true;
    
        setNamePathLog(serverP, name, filesPath, logFileName);
    }

    return success;
}



static void
createSwitchFromOsSocket(TOsSocket      const osSocket,
                         TChanSwitch ** const chanSwitchPP,
                         const char **  const errorP) {

#ifdef _WIN32
    ChanSwitchWinCreateWinsock(osSocket, chanSwitchPP, errorP);
#else
    ChanSwitchUnixCreateFd(osSocket, chanSwitchPP, errorP);
#endif
}



static void
createChannelFromOsSocket(TOsSocket     const osSocket,
                          TChannel **   const channelPP,
                          void **       const channelInfoPP,
                          const char ** const errorP) {

#ifdef _WIN32
    ChannelWinCreateWinsock(osSocket, channelPP,
                            (struct abyss_win_chaninfo**)channelInfoPP,
                            errorP);
#else
    ChannelUnixCreateFd(osSocket, channelPP,
                        (struct abyss_unix_chaninfo**)channelInfoPP,
                        errorP);
#endif
}



abyss_bool
ServerCreateSocket(TServer *    const serverP,
                   const char * const name,
                   TOsSocket    const socketFd,
                   const char * const filesPath,
                   const char * const logFileName) {

    bool success;
    TChanSwitch * chanSwitchP;
    const char * error;

    createSwitchFromOsSocket(socketFd, &chanSwitchP, &error);

    if (error) {
        TraceMsg(error);
        success = false;
        xmlrpc_strfree(error);
    } else {
        bool const noAcceptFalse = false;
        bool const userChanSwitchFalse = false;

        const char * error;

        createServer(&serverP->srvP, noAcceptFalse,
                     chanSwitchP, userChanSwitchFalse,
                     0, &error);

        if (error) {
            TraceMsg(error);
            success = false;
            xmlrpc_strfree(error);
        } else {
            success = true;
            
            setNamePathLog(serverP, name, filesPath, logFileName);
        }
        if (!success)
            ChanSwitchDestroy(chanSwitchP);
    }

    return success;
}



abyss_bool
ServerCreateNoAccept(TServer *    const serverP,
                     const char * const name,
                     const char * const filesPath,
                     const char * const logFileName) {

    bool const noAcceptTrue = true;
    bool const userChanSwitchFalse = false;

    bool success;
    const char * error;

    createServer(&serverP->srvP, noAcceptTrue,
                 NULL, userChanSwitchFalse,
                 0, &error);

    if (error) {
        TraceMsg(error);
        success = false;
        xmlrpc_strfree(error);
    } else {
        success = true;
        
        setNamePathLog(serverP, name, filesPath, logFileName);
    }
    return success;
}



void
ServerCreateSwitch(TServer *     const serverP,
                   TChanSwitch * const chanSwitchP,
                   const char ** const errorP) {
    
    bool const noAcceptFalse = false;
    bool const userChanSwitchTrue = true;

    assert(serverP);
    assert(chanSwitchP);

    createServer(&serverP->srvP, noAcceptFalse,
                 chanSwitchP, userChanSwitchTrue,
                 0, errorP);
}



void
ServerCreateSocket2(TServer *     const serverP,
                    TSocket *     const socketP,
                    const char ** const errorP) {
    
    TChanSwitch * const chanSwitchP = SocketGetChanSwitch(socketP);

    assert(socketP);

    if (!chanSwitchP)
        xmlrpc_asprintf(
            errorP, "Socket is not a listening socket.  "
            "You should use ServerCreateSwitch() instead, anyway.");
    else
        ServerCreateSwitch(serverP, chanSwitchP, errorP);
}



static void
terminateHandlers(TList * const handlersP) {
/*----------------------------------------------------------------------------
   Terminate all handlers in the list '*handlersP'.

   I.e. call each handler's terminate function.
-----------------------------------------------------------------------------*/
    if (handlersP->item) {
        unsigned int i;
        for (i = handlersP->size; i > 0; --i) {
            struct uriHandler * const handlerP = handlersP->item[i-1];
            if (handlerP->term)
                handlerP->term(handlerP->userdata);
        }
    }
}



void
ServerFree(TServer * const serverP) {

    struct _TServer * const srvP = serverP->srvP;

    if (srvP->weCreatedChanSwitch)
        ChanSwitchDestroy(srvP->chanSwitchP);

    xmlrpc_strfree(srvP->name);

    terminateHandlers(&srvP->handlers);

    ListFree(&srvP->handlers);

    HandlerDestroy(srvP->builtinHandlerP);
    
    logClose(srvP);

    if (srvP->logfilename)
        xmlrpc_strfree(srvP->logfilename);

    free(srvP);
}



void
ServerSetName(TServer *    const serverP,
              const char * const name) {

    xmlrpc_strfree(serverP->srvP->name);
    
    serverP->srvP->name = strdup(name);
}



void
ServerSetFilesPath(TServer *    const serverP,
                   const char * const filesPath) {

    HandlerSetFilesPath(serverP->srvP->builtinHandlerP, filesPath);
}



void
ServerSetLogFileName(TServer *    const serverP,
                     const char * const logFileName) {
    
    struct _TServer * const srvP = serverP->srvP;

    if (srvP->logfilename)
        xmlrpc_strfree(srvP->logfilename);
    
    srvP->logfilename = strdup(logFileName);
}



void
ServerSetKeepaliveTimeout(TServer *       const serverP,
                          xmlrpc_uint32_t const keepaliveTimeout) {

    serverP->srvP->keepalivetimeout = MAX(keepaliveTimeout, 1);
}



void
ServerSetKeepaliveMaxConn(TServer *       const serverP,
                          xmlrpc_uint32_t const keepaliveMaxConn) {

    serverP->srvP->keepalivemaxconn = MAX(keepaliveMaxConn, 1);
}



void
ServerSetTimeout(TServer *       const serverP,
                 xmlrpc_uint32_t const timeout) {

    serverP->srvP->timeout = timeout;
}



void
ServerSetAdvertise(TServer *  const serverP,
                   abyss_bool const advertise) {

    serverP->srvP->advertise = advertise;
}



void
ServerSetMimeType(TServer *  const serverP,
                  MIMEType * const mimeTypeP) {
    
    HandlerSetMimeType(serverP->srvP->builtinHandlerP, mimeTypeP);
}



void
ServerSetMaxConn(TServer *    const serverP,
                 unsigned int const maxConn) {

    if (maxConn > 0) {
        serverP->srvP->maxConn = maxConn;
    }
}



void
ServerSetMaxConnBacklog(TServer *    const serverP,
                        unsigned int const maxConnBacklog) {

    if (maxConnBacklog > 0) {
        serverP->srvP->maxConnBacklog = maxConnBacklog;
    }
}



static URIHandler2
makeUriHandler2(const struct uriHandler * const handlerP) {

    URIHandler2 retval;

    retval.init       = handlerP->init;
    retval.term       = handlerP->term;
    retval.handleReq2 = handlerP->handleReq2;
    retval.handleReq1 = handlerP->handleReq1;
    retval.userdata   = handlerP->userdata;

    return retval;
}



static void
runUserHandler(TSession *        const sessionP,
               struct _TServer * const srvP) {

    abyss_bool handled;
    int i;
    
    for (i = srvP->handlers.size-1, handled = false;
         i >= 0 && !handled;
         --i) {
        const struct uriHandler * const handlerP = srvP->handlers.item[i];
        
        if (handlerP->handleReq3)
            handlerP->handleReq3(handlerP->userdata, sessionP, &handled);
        if (handlerP->handleReq2) {
            URIHandler2 handler2 = makeUriHandler2(handlerP);
            handlerP->handleReq2(&handler2, sessionP, &handled);
        } else if (handlerP->handleReq1)
            handled = handlerP->handleReq1(sessionP);
    }

    assert(srvP->defaultHandler);
    
    if (!handled)
        srvP->defaultHandler(sessionP);
}



static void
handleReqTooNewHttpVersion(TSession * const sessionP) {

    const char * msg;

    ResponseStatus(sessionP, 505);

    xmlrpc_asprintf(&msg, "Request is in HTTP Version %u"
                    "We understand only HTTP 1",
                    sessionP->version.major);
    
    ResponseError2(sessionP, msg);
    
    xmlrpc_strfree(msg);
}



static void
handleReqInvalidURI(TSession * const sessionP) {

    ResponseStatus(sessionP, 400);

    ResponseError2(sessionP, "Invalid URI");
}



static void
processRequestFromClient(TConn *  const connectionP,
                         bool     const lastReqOnConn,
                         uint32_t const timeout,
                         bool *   const keepAliveP) {
/*----------------------------------------------------------------------------
   Get and execute one HTTP request from client connection *connectionP,
   through the connection buffer.  I.e. Some of the request may already be in
   the connection buffer, and we may leave some of later requests in the
   connection buffer.

   In fact, because of timing considerations, we assume the client has begun
   sending the request, which as a practical matter means Caller has already
   deposited some of it in the connection buffer.

   If there isn't one full request in the buffer now, we wait for one full
   request to come through the buffer, up to 'timeout'.

   We return as *keepAliveP whether Caller should keep the connection
   alive for a while for possible future requests from the client, based
   on 'lastReqOnConn' and the content of the HTTP request.

   Executing the request consists primarily of calling the URI handlers that
   are associated with the connection (*connectionP), passing each the request
   information we read.  Each handler can respond according to the HTTP method
   (GET, POST, etc) and URL etc, and that response may be either to
   execute the request and send the response or refuse the request and let
   us call the next one in the list.
-----------------------------------------------------------------------------*/
    TSession session;
    const char * error;
    uint16_t httpErrorCode;

    RequestInit(&session, connectionP);

    session.serverDeniesKeepalive = lastReqOnConn;
        
    RequestRead(&session, timeout, &error, &httpErrorCode);

    if (error) {
        ResponseStatus(&session, httpErrorCode);
        ResponseError2(&session, error);
        xmlrpc_strfree(error);
    } else {
        if (session.version.major >= 2)
            handleReqTooNewHttpVersion(&session);
        else if (!RequestValidURI(&session))
            handleReqInvalidURI(&session);
        else
            runUserHandler(&session, connectionP->server->srvP);
    }

    assert(session.status != 0);

    if (session.responseStarted)
        ResponseWriteEnd(&session);
    else
        ResponseError(&session);

    *keepAliveP = HTTPKeepalive(&session);

    SessionLog(&session);

    RequestFree(&session);
}



static TThreadProc serverFunc;

static void
serverFunc(void * const userHandle) {
/*----------------------------------------------------------------------------
   Do server stuff on one connection.  At its simplest, this means do
   one HTTP request.  But with keepalive, it can be many requests.
-----------------------------------------------------------------------------*/
    TConn *           const connectionP = userHandle;
    struct _TServer * const srvP = connectionP->server->srvP;

    unsigned int requestCount;
        /* Number of requests we've handled so far on this connection */
    bool connectionDone;
        /* No more need for this HTTP connection */

    trace(srvP, "Thread starting to handle requests on a new connection.  "
          "PID = %d", getpid());

    requestCount = 0;
    connectionDone = false;

    while (!connectionDone) {
        bool timedOut, eof;
        const char * readError;
        
        /* Wait for and get beginning (at least ) of next request.  We do
           this separately from getting the rest of the request because we
           treat dead time between requests differently from dead time in
           the middle of a request.
        */
        ConnRead(connectionP, srvP->keepalivetimeout,
                 &timedOut, &eof, &readError);

        if (srvP->terminationRequested) {
            connectionDone = true;
        } else if (readError) {
            TraceMsg("Failed to read from Abyss connection.  %s", readError);
            xmlrpc_strfree(readError);
            connectionDone = true;
        } else if (timedOut) {
            connectionDone = true;
        } else if (eof) {
            connectionDone = true;
        } else {
            bool const lastReqOnConn =
                requestCount + 1 >= srvP->keepalivemaxconn;

            bool keepalive;

            trace(srvP, "HTTP request %u at least partially received.  "
                  "Receiving the rest and processing", requestCount);
            
            processRequestFromClient(connectionP, lastReqOnConn, srvP->timeout,
                                     &keepalive);

            trace(srvP, "Done processing the HTTP request.  Keepalive = %s",
                  keepalive ? "YES" : "NO");
            
            ++requestCount;

            if (!keepalive)
                connectionDone = true;
            
            /**************** Must adjust the read buffer *****************/
            ConnReadInit(connectionP);
        }
    }
    trace(srvP, "PID %d done with connection", getpid());
}



/* This is the maximum amount of stack space, in bytes, serverFunc()
   itself requires -- not counting what the user's request handler
   (which serverFunc() calls) requires.
*/
#define SERVER_FUNC_STACK 1024



static void
createSwitchFromPortNum(unsigned short const portNumber,
                        TChanSwitch ** const chanSwitchPP,
                        const char **  const errorP) {

#ifdef _WIN32
    ChanSwitchWinCreate(portNumber, chanSwitchPP, errorP);
#else
    ChanSwitchUnixCreate(portNumber, chanSwitchPP, errorP);
#endif
}    



static void
createChanSwitch(struct _TServer * const srvP,
                 const char **     const errorP) {

    TChanSwitch * chanSwitchP;
    const char * error;
    
    /* Not valid to call this when channel switch already exists: */
    assert(srvP->chanSwitchP == NULL);

    createSwitchFromPortNum(srvP->port, &chanSwitchP, &error);

    if (error) {
        xmlrpc_asprintf(errorP,
                        "Can't create channel switch.  %s", error);
        xmlrpc_strfree(error);
    } else {
        *errorP = NULL;
        
        srvP->weCreatedChanSwitch = true;
        srvP->chanSwitchP         = chanSwitchP;
    }
}



void
ServerInit2(TServer *     const serverP,
            const char ** const errorP) {
/*----------------------------------------------------------------------------
   Initialize a server to accept connections.

   Do not confuse this with creating the server -- ServerCreate().

   Not necessary or valid with a server that doesn't accept connections (i.e.
   user supplies the channels (TCP connections)).
-----------------------------------------------------------------------------*/
    struct _TServer * const srvP = serverP->srvP;
    
    if (!srvP->serverAcceptsConnections)
        xmlrpc_asprintf(errorP,
                        "ServerInit() is not valid on a server that doesn't "
                        "accept connections "
                        "(i.e. created with ServerCreateNoAccept)");
    else {
        *errorP = NULL;  /* initial value */

        if (!srvP->chanSwitchP) {
            const char * error;
            createChanSwitch(srvP, &error);

            if (error) {
                xmlrpc_asprintf(errorP, "Unable to create a channel switch "
                                "for the server.  %s", error);
                xmlrpc_strfree(error);
            }
        }

        if (!*errorP) {
            const char * error;

            assert(srvP->chanSwitchP);

            ChanSwitchListen(srvP->chanSwitchP, srvP->maxConnBacklog, &error);

            if (error) {
                xmlrpc_asprintf(errorP,
                                "Failed to listen on bound socket.  %s",
                                error);
                xmlrpc_strfree(error);
            } else {
                srvP->readyToAccept = true;
            }
        }
    }
}



void
ServerInit(TServer * const serverP) {

    const char * error;

    ServerInit2(serverP, &error);

    if (error) {
        TraceExit("ServerInit() failed.  %s", error);
        xmlrpc_strfree(error);
    }
}



/* We don't do any locking on the outstanding connections list, so 
   we must make sure that only the master thread (the one that listens
   for connections) ever accesses it.

   That's why when a thread completes, it places the connection in
   "finished" status, but doesn't destroy the connection.
*/

typedef struct {

    TConn * firstP;
    unsigned int count;
        /* Redundant with 'firstP', for quick access */
} outstandingConnList;



static void
createOutstandingConnList(outstandingConnList ** const listPP) {

    outstandingConnList * listP;

    MALLOCVAR_NOFAIL(listP);

    listP->firstP = NULL;  /* empty list */
    listP->count = 0;

    *listPP = listP;
}



static void
destroyOutstandingConnList(outstandingConnList * const listP) {

    assert(listP->firstP == NULL);
    assert(listP->count == 0);

    free(listP);
}



static void
addToOutstandingConnList(outstandingConnList * const listP,
                         TConn *               const connectionP) {

    connectionP->nextOutstandingP = listP->firstP;

    listP->firstP = connectionP;

    ++listP->count;
}



static void
freeFinishedConns(outstandingConnList * const listP) {
/*----------------------------------------------------------------------------
   Garbage-collect the resources associated with connections that are
   finished with their jobs.  Thread resources, connection pool
   descriptor, etc.
-----------------------------------------------------------------------------*/
    TConn ** pp;

    pp = &listP->firstP;

    while (*pp) {
        TConn * const connectionP = (*pp);

        ThreadUpdateStatus(connectionP->threadP);
        
        if (connectionP->finished) {
            /* Take it out of the list */
            *pp = connectionP->nextOutstandingP;
            --listP->count;
            
            ConnWaitAndRelease(connectionP);
        } else {
            /* Move to next connection in list */
            pp = &connectionP->nextOutstandingP;
        }
    }
}



static void
waitForConnectionFreed(outstandingConnList * const outstandingConnListP
                       ATTR_UNUSED) {
/*----------------------------------------------------------------------------
  Wait for a connection descriptor in 'connectionPool' to be probably
  freed.
-----------------------------------------------------------------------------*/

    /* TODO: We should do something more sophisticated here.  For pthreads,
       we can have a thread signal us by semaphore when it terminates.
       For fork, we might be able to use the "done" handler argument
       to ConnCreate() to get interrupted when the death of a child
       signal happens.
    */

    xmlrpc_millisecond_sleep(2000);
}



static void
waitForNoConnections(outstandingConnList * const outstandingConnListP) {

    while (outstandingConnListP->firstP) {
        freeFinishedConns(outstandingConnListP);
    
        if (outstandingConnListP->firstP)
            waitForConnectionFreed(outstandingConnListP);
    }
}



static void
waitForConnectionCapacity(outstandingConnList * const outstandingConnListP,
                          unsigned int          const maxConn) {
/*----------------------------------------------------------------------------
   Wait until there are fewer than 'maxConn' connections in progress.
-----------------------------------------------------------------------------*/
    while (outstandingConnListP->count >= maxConn) {
        freeFinishedConns(outstandingConnListP);
        if (outstandingConnListP->firstP)
            waitForConnectionFreed(outstandingConnListP);
    }
}



static void
interruptChannels(outstandingConnList * const outstandingConnListP) {
/*----------------------------------------------------------------------------
   Get every thread that is waiting to read a request or write a response
   for a connection to stop waiting.
-----------------------------------------------------------------------------*/
    TConn * connP;

    for (connP = outstandingConnListP->firstP;
         connP; connP = connP->nextOutstandingP) {

        if (connP->finished) {
            /* The connection couldn't be waiting on the channel, and the
               channel probably doesn't even exit anymore.
            */
        } else 
            ChannelInterrupt(connP->channelP);
    }
}



#ifndef _WIN32
void
ServerHandleSigchld(pid_t const pid) {

    ThreadHandleSigchld(pid);
}
#endif



void
ServerUseSigchld(TServer * const serverP) {

    struct _TServer * const srvP = serverP->srvP;
    
    srvP->useSigchld = true;
}



static TThreadDoneFn destroyChannel;

static void
destroyChannel(void * const userHandle) {
/*----------------------------------------------------------------------------
   This is a "connection done" function for the connection the server
   serves.  It gets called some time after the connection has done its
   thing.  Its job is to clean up stuff the server created for use by
   the connection, but the server can't clean up because the
   connection might be processed asynchronously in a background
   thread.

   To wit, we destroy the connection's channel and release the memory
   for the information about that channel.
-----------------------------------------------------------------------------*/
    TConn * const connectionP = userHandle;

    ChannelDestroy(connectionP->channelP);
    free(connectionP->channelInfoP);
}



static void
processNewChannel(TServer *             const serverP,
                  TChannel *            const channelP,
                  void *                const channelInfoP,
                  outstandingConnList * const outstandingConnListP,
                  const char **         const errorP) {

    struct _TServer * const srvP = serverP->srvP;
                      
    TConn * connectionP;
    const char * error;

    freeFinishedConns(outstandingConnListP);
            
    trace(srvP, "Waiting for there to be fewer than the maximum "
          "%u sessions in progress",
          srvP->maxConn);

    waitForConnectionCapacity(outstandingConnListP, srvP->maxConn);
            
    ConnCreate(&connectionP, serverP, channelP, channelInfoP,
               &serverFunc,
               SERVER_FUNC_STACK + srvP->uriHandlerStackSize,
               &destroyChannel, ABYSS_BACKGROUND,
               srvP->useSigchld,
               &error);
    if (!error) {
        addToOutstandingConnList(outstandingConnListP, connectionP);
        ConnProcess(connectionP);
        /* When connection is done (which could be later, courtesy of a
           background thread), destroyChannel() will destroy *channelP.
        */
        *errorP = NULL;
    } else {
        xmlrpc_asprintf(
            errorP, "Failed to create an Abyss connection.  %s", error);
        xmlrpc_strfree(error);
    }
}



static void
acceptAndProcessNextConnection(
    TServer *             const serverP,
    outstandingConnList * const outstandingConnListP,
    const char **         const errorP) {

    struct _TServer * const srvP = serverP->srvP;

    const char * error;
    TChannel * channelP;
    void * channelInfoP;

    trace(srvP, "Waiting for a new channel from channel switch");

    assert(srvP->readyToAccept);
    assert(srvP->chanSwitchP);
        
    ChanSwitchAccept(srvP->chanSwitchP, &channelP, &channelInfoP, &error);
    
    if (error) {
        xmlrpc_asprintf(errorP,
                        "Failed to accept the next connection from a client "
                        "at the channel level.  %s", error);
        xmlrpc_strfree(error);
    } else {
        if (channelP) {
            const char * error;

            trace(srvP, "Got a new channel from channel switch");

            processNewChannel(serverP, channelP, channelInfoP,
                              outstandingConnListP, &error);

            if (error) {
                xmlrpc_asprintf(errorP, "Failed to use new channel %lx",
                                (unsigned long) channelP);
                ChannelDestroy(channelP);
                free(channelInfoP);
            } else {
                trace(srvP, "successfully processed newly accepted channel");
                /* Connection created above will destroy *channelP
                   and *channelInfoP as it terminates.
                */
            }
        } else {
            /* Accept function was interrupted before it got a connection */
            trace(srvP, "Wait for new channel from switch was interrupted");
            *errorP = NULL;
        }
    }
}



static void 
serverRun2(TServer *     const serverP,
           const char ** const errorP) {

    struct _TServer * const srvP = serverP->srvP;
    outstandingConnList * outstandingConnListP;

    createOutstandingConnList(&outstandingConnListP);

    *errorP = NULL;  /* initial value */

    trace(srvP, "Starting main connection accepting loop");
    
    while (!srvP->terminationRequested && !*errorP)
        acceptAndProcessNextConnection(serverP, outstandingConnListP, errorP);

    trace(srvP, "Main connection accepting loop is done");

    if (!*errorP) {
        trace(srvP, "Interrupting and waiting for %u existing connections "
              "to finish",
              outstandingConnListP->count);

        interruptChannels(outstandingConnListP);

        waitForNoConnections(outstandingConnListP);
    
        trace(srvP, "No connections left");

        destroyOutstandingConnList(outstandingConnListP);
    }
}



void 
ServerRun(TServer * const serverP) {

    struct _TServer * const srvP = serverP->srvP;

    trace(srvP, "%s entered", __FUNCTION__);

    if (!srvP->serverAcceptsConnections)
        TraceMsg("This server is not set up to accept connections "
                 "on its own, so you can't use ServerRun().  "
                 "Try ServerRunConn()");
    else if (!srvP->readyToAccept)
        TraceMsg("You cannot run this server until you initialize it to "
                 "accept connections, with ServerInit()");
    else {
        const char * error;

        serverRun2(serverP, &error);

        if (error) {
            TraceMsg("Server failed.  %s", error);

            xmlrpc_strfree(error);
        }
    }
    trace(srvP, "%s exiting", __FUNCTION__);
}



static void
serverRunChannel(TServer *     const serverP,
                 TChannel *    const channelP,
                 void *        const channelInfoP,
                 const char ** const errorP) {
/*----------------------------------------------------------------------------
   Do the HTTP transaction on the channel 'channelP'.
   (channel must be at the beginning of the HTTP request -- nothing having
   been read or written yet).

   channelInfoP == NULL means no channel info supplied.
-----------------------------------------------------------------------------*/
    struct _TServer * const srvP = serverP->srvP;

    TConn * connectionP;
    const char * error;

    trace(srvP, "%s entered", __FUNCTION__);

    srvP->keepalivemaxconn = 1;

    ConnCreate(&connectionP, 
               serverP, channelP, channelInfoP,
               &serverFunc, SERVER_FUNC_STACK + srvP->uriHandlerStackSize,
               NULL, ABYSS_FOREGROUND, srvP->useSigchld,
               &error);
    if (error) {
        xmlrpc_asprintf(errorP, "Couldn't create HTTP connection out of "
                        "channel (connected socket).  %s", error);
        xmlrpc_strfree(error);
    } else {
        *errorP = NULL;

        ConnProcess(connectionP);

        ConnWaitAndRelease(connectionP);
    }
    trace(srvP, "%s exiting", __FUNCTION__);
}



void
ServerRunChannel(TServer *     const serverP,
                 TChannel *    const channelP,
                 void *        const channelInfoP,
                 const char ** const errorP) {
/*----------------------------------------------------------------------------
  Do the HTTP transaction on the channel 'channelP'.

  (channel must be at the beginning of the HTTP request -- nothing having
  been read or written yet).
-----------------------------------------------------------------------------*/
    struct _TServer * const srvP = serverP->srvP;

    trace(srvP, "%s entered", __FUNCTION__);

    if (srvP->serverAcceptsConnections)
        xmlrpc_asprintf(errorP,
                        "This server is configured to accept connections on "
                        "its own socket.  "
                        "Try ServerRun() or ServerCreateNoAccept().");
    else
        serverRunChannel(serverP, channelP, channelInfoP, errorP);

    trace(srvP, "%s exiting", __FUNCTION__);
}



void
ServerRunConn2(TServer *     const serverP,
               TSocket *     const connectedSocketP,
               const char ** const errorP) {
/*----------------------------------------------------------------------------
   Do the HTTP transaction on the TCP connection on the socket
   *connectedSocketP.
   (socket must be connected state, with nothing having been read or
   written on the connection yet).
-----------------------------------------------------------------------------*/
    TChannel * const channelP = SocketGetChannel(connectedSocketP);

    if (!channelP)
        xmlrpc_asprintf(errorP, "The socket supplied is not a connected "
                        "socket.  You should use ServerRunChannel() instead, "
                        "anyway.");
    else
        ServerRunChannel(serverP,
                         channelP, SocketGetChannelInfo(connectedSocketP),
                         errorP);
}



void
ServerRunConn(TServer * const serverP,
              TOsSocket const connectedOsSocket) {

    TChannel * channelP;
    void * channelInfoP;
    const char * error;

    createChannelFromOsSocket(connectedOsSocket,
                              &channelP, &channelInfoP, &error);
    if (error) {
        TraceExit("Unable to use supplied socket");
        xmlrpc_strfree(error);
    } else {
        const char * error;

        ServerRunChannel(serverP, channelP, channelInfoP, &error);

        if (error) {
            TraceExit("Failed to run server on connection on file "
                      "descriptor %d.  %s", connectedOsSocket, error);
            xmlrpc_strfree(error);
        }
        ChannelDestroy(channelP);
        free(channelInfoP);
    }
}



void
ServerRunOnce(TServer * const serverP) {
/*----------------------------------------------------------------------------
   Accept a connection from the channel switch and do the HTTP
   transaction that comes over it.

   If no connection is presently waiting at the switch, wait for one.
   But return immediately if we receive a signal during the wait.
-----------------------------------------------------------------------------*/
    struct _TServer * const srvP = serverP->srvP;

    trace(srvP, "%s entered", __FUNCTION__);

    if (!srvP->serverAcceptsConnections)
        TraceMsg("This server is not set up to accept connections "
                 "on its own, so you can't use ServerRun().  "
                 "Try ServerRunConn()");
    else if (!srvP->readyToAccept)
        TraceMsg("You cannot run this server until you initialize it to "
                 "accept connections, with ServerInit()");
    else {
        const char * error;
        TChannel *   channelP;
        void *       channelInfoP;
    
        srvP->keepalivemaxconn = 1;

        assert(srvP->chanSwitchP);

        ChanSwitchAccept(srvP->chanSwitchP, &channelP, &channelInfoP, &error);
        if (error) {
            TraceMsg("Failed to accept the next connection from a client "
                     "at the channel level.  %s", error);
            xmlrpc_strfree(error);
        } else {
            if (channelP) {
                const char * error;

                serverRunChannel(serverP, channelP, channelInfoP, &error);

                if (error) {
                    const char * peerDesc;
                    ChannelFormatPeerInfo(channelP, &peerDesc);
                    TraceExit("Got a connection from '%s', but failed to "
                              "run server on it.  %s", peerDesc, error);
                    xmlrpc_strfree(peerDesc);
                    xmlrpc_strfree(error);
                }
                ChannelDestroy(channelP);
                free(channelInfoP);
            }
        }
    }
    trace(srvP, "%s exiting", __FUNCTION__);
}



void
ServerRunOnce2(TServer *           const serverP,
               enum abyss_foreback const foregroundBackground ATTR_UNUSED) {
/*----------------------------------------------------------------------------
   This is a backward compatibility interface to ServerRunOnce().

   'foregroundBackground' is meaningless.  We always process the
   connection in the foreground.  The parameter exists because we once
   thought we could do them in the background, but we really can't do
   that in any clean way.  If Caller wants background execution, he can
   spin his own thread or process to call us.  It makes much more sense
   in Caller's context.
-----------------------------------------------------------------------------*/
    ServerRunOnce(serverP);
}



static void
setGroups(const char ** const errorP) {

#if HAVE_SETGROUPS   
    if (setgroups(0, NULL) == -1)
        xmlrpc_asprintf(errorP, "setgroups() errno = %d (%s)",
                        errno, strerror(errno));
#else
    *errorP = NULL;
#endif
}



void
ServerDaemonize(TServer * const serverP) {
/*----------------------------------------------------------------------------
   Turn Caller into a daemon (i.e. fork a child, then exit; the child
   returns to Caller).

   NOTE: It's ridiculous, but conventional, for us to do this.  It's
   ridiculous because the task of daemonizing is not something
   particular to Abyss.  It ought to be done by a higher level.  In
   fact, it should be done before the Abyss server program is even
   execed.  The user should run a "daemonize" program that creates a
   daemon which execs the Abyss server program.
-----------------------------------------------------------------------------*/
    struct _TServer * const srvP = serverP->srvP;

#ifndef _WIN32
    /* Become a daemon */
    switch (fork()) {
    case 0:
        break;
    case -1:
        TraceExit("Unable to become a daemon");
    default:
        /* We are the parent */
        exit(0);
    }
    
    setsid();

    /* Change the current user if we are root */
    if (getuid()==0) {
        const char * error;
        if (srvP->uid == (uid_t)-1)
            TraceExit("Can't run under root privileges.  "
                      "Please add a User option in your "
                      "Abyss configuration file.");

        setGroups(&error);

        if (error) {
            TraceExit("Failed to set groups.  %s", error);
            xmlrpc_strfree(error);
        }

        if (srvP->gid != (gid_t)-1)
            if (setgid(srvP->gid)==(-1))
                TraceExit("Failed to change the group.");
        
        if (setuid(srvP->uid) == -1)
            TraceExit("Failed to change the user.");
    }
    
    if (srvP->pidfileP) {
        char z[16];
    
        sprintf(z, "%d", getpid());
        FileWrite(srvP->pidfileP, z, strlen(z));
        FileClose(srvP->pidfileP);
    }
#endif  /* _WIN32 */
}



static void
serverAddHandler(TServer *     const serverP,
                 initHandlerFn       init,
                 termHandlerFn       term,
                 URIHandler          handleReq1,
                 handleReq2Fn        handleReq2,
                 handleReq3Fn        handleReq3,
                 void *        const userdata,
                 size_t        const handleReqStackSizeReq,
                 abyss_bool *  const successP) {

    struct _TServer * const srvP = serverP->srvP;
    size_t handleReqStackSize =
        handleReqStackSizeReq ? handleReqStackSizeReq : 128*1024;

    struct uriHandler * handlerP;

    MALLOCVAR(handlerP);
    if (handlerP == NULL)
        *successP = false;
    else {
        handlerP->init       = init;
        handlerP->term       = term;
        handlerP->handleReq1 = handleReq1;
        handlerP->handleReq2 = handleReq2;
        handlerP->handleReq3 = handleReq3;
        handlerP->userdata   = userdata;

        srvP->uriHandlerStackSize =
            MAX(srvP->uriHandlerStackSize, handleReqStackSize);
        
        if (handlerP->init == NULL)
            *successP = true;
        else {
            URIHandler2 handler2 = makeUriHandler2(handlerP);
            handlerP->init(&handler2, successP);
        }
        if (*successP)
            *successP = ListAdd(&serverP->srvP->handlers, handlerP);

        if (!*successP)
            free(handlerP);
    }
}



void
ServerAddHandler3(TServer *                        const serverP,
                  const struct ServerReqHandler3 * const handlerP,
                  abyss_bool *                     const successP) {

    serverAddHandler(serverP, NULL, handlerP->term, NULL, NULL,
                     handlerP->handleReq, handlerP->userdata,
                     handlerP->handleReqStackSize, successP);
}



void
ServerAddHandler2(TServer *     const serverP,
                  URIHandler2 * const handlerArgP,
                  abyss_bool *  const successP) {

    /* This generation of the URI handler interface is strange because
       it went through an unfortunate evolution.  So it halfway looks like
       the use supplies a handler object and Abyss calls its methods, and
       halfway looks like the user simply describes his handler.

       Abyss calls handleReq2 with a pointer to a URIHandler2 like the
       one which is our argument, but it isn't the same one.  User can
       discard *handlerArgP as soon as we return.
    */
    
    serverAddHandler(serverP,
                     handlerArgP->init,
                     handlerArgP->term,
                     NULL,
                     handlerArgP->handleReq2,
                     NULL,
                     handlerArgP->userdata,
                     0,
                     successP);
}



abyss_bool
ServerAddHandler(TServer *  const serverP,
                 URIHandler const function) {

    URIHandler2 handler;
    abyss_bool success;

    handler.init       = NULL;
    handler.term       = NULL;
    handler.userdata   = NULL;
    handler.handleReq2 = NULL;
    handler.handleReq1 = function;

    ServerAddHandler2(serverP, &handler, &success);

    return success;
}



/* This is the maximum amount of stack we allow a user's default URI
   handler to use.  (If he exceeds this, results are undefined).

   We really ought to provide user a way to set this, as he can for
   his non-default URI handlers.
*/
#define USER_DEFAULT_HANDLER_STACK 128*1024

void
ServerDefaultHandler(TServer *  const serverP,
                     URIHandler const handler) {

    struct _TServer * const srvP = serverP->srvP;

    if (handler) {
        srvP->defaultHandler = handler;
        srvP->uriHandlerStackSize =
            MAX(srvP->uriHandlerStackSize, USER_DEFAULT_HANDLER_STACK);
    } else {
        srvP->defaultHandler = HandlerDefaultBuiltin;
        srvP->defaultHandlerContext = srvP->builtinHandlerP;
        srvP->uriHandlerStackSize =
            MAX(srvP->uriHandlerStackSize, HandlerDefaultBuiltinStack);
    }
}



void
LogWrite(TServer *    const serverP,
         const char * const msg) {

    struct _TServer * const srvP = serverP->srvP;

    if (!srvP->logfileisopen && srvP->logfilename) {
        const char * error;
        logOpen(srvP, &error);

        if (error) {
            TraceMsg("Failed to open log file.  %s", error);

            xmlrpc_strfree(error);
        }
    }
    if (srvP->logfileisopen) {
        const char * const lbr = "\n";
        srvP->logLockP->acquire(srvP->logLockP);
        FileWrite(srvP->logfileP, msg, strlen(msg));
        FileWrite(srvP->logfileP, lbr, strlen(lbr));
        
        srvP->logLockP->release(srvP->logLockP);
    }
}
/*******************************************************************************
**
** server.c
**
** This file is part of the ABYSS Web server project.
**
** Copyright (C) 2000 by Moez Mahfoudh <mmoez@bigfoot.com>.
** All rights reserved.
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
******************************************************************************/
