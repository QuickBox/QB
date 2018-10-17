#include "xmlrpc_config.h"
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/abyss.h */
#include <cstdlib>
#include <string>
#include <memory>
#include <signal.h>
#include <errno.h>
#include <iostream>
#if !MSVCRT
#  include <sys/wait.h>
#endif

#include "assertx.hpp"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/girerr.hpp"
using girerr::error;
using girerr::throwf;
#include "xmlrpc-c/base.h"
#include "xmlrpc-c/util.h"
#include "xmlrpc-c/base.hpp"
#include "xmlrpc-c/abyss.h"
#include "xmlrpc-c/server_abyss.h"
#include "xmlrpc-c/registry.hpp"
#include "xmlrpc-c/env_wrap.hpp"

#include "xmlrpc-c/server_abyss.hpp"

using namespace std;
using namespace xmlrpc_c;

namespace xmlrpc_c {

namespace {


static void 
sigterm(int const signalClass) {

    cerr << "Signal of Class " << signalClass << " received.  Exiting" << endl;

    exit(1);
}



static void 
sigchld(int const ASSERT_ONLY_ARG(signalClass)) {
/*----------------------------------------------------------------------------
   This is a signal handler for a SIGCHLD signal (which informs us that
   one of our child processes has terminated).

   The only child processes we have are those that belong to the Abyss
   server (and then only if the Abyss server was configured to use
   forking as a threading mechanism), so we respond by passing the
   signal on to the Abyss server.  And reaping the dead child.
-----------------------------------------------------------------------------*/
#ifndef _WIN32
    // Reap zombie children / report to Abyss until there aren't any more.

    bool zombiesExist;
    bool error;

    assert(signalClass == SIGCHLD);
    
    zombiesExist = true;  // initial assumption
    error = false;  // no error yet
    while (zombiesExist && !error) {
        int status;
        pid_t const pid = waitpid((pid_t) -1, &status, WNOHANG);
    
        if (pid == 0)
            zombiesExist = false;
        else if (pid < 0) {
            /* because of ptrace */
            if (errno == EINTR) {
                // This is OK - it's a ptrace notification
            } else
                error = true;
        } else
            ServerHandleSigchld(pid);
    }
#endif /* _WIN32 */
}



struct signalHandlers {
#ifndef WIN32
    struct sigaction term;
    struct sigaction int_;
    struct sigaction hup;
    struct sigaction usr1;
    struct sigaction pipe;
    struct sigaction chld;
#else
    int dummy;
#endif
};



void
setupSignalHandlers(struct signalHandlers * const oldHandlersP) {
#ifndef _WIN32
    struct sigaction mysigaction;
   
    sigemptyset(&mysigaction.sa_mask);
    mysigaction.sa_flags = 0;

    /* These signals abort the program, with tracing */
    mysigaction.sa_handler = sigterm;
    sigaction(SIGTERM, &mysigaction, &oldHandlersP->term);
    sigaction(SIGINT,  &mysigaction, &oldHandlersP->int_);
    sigaction(SIGHUP,  &mysigaction, &oldHandlersP->hup);
    sigaction(SIGUSR1, &mysigaction, &oldHandlersP->usr1);

    /* This signal indicates connection closed in the middle */
    mysigaction.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &mysigaction, &oldHandlersP->pipe);
   
    /* This signal indicates a child process (request handler) has died */
    mysigaction.sa_handler = sigchld;
    sigaction(SIGCHLD, &mysigaction, &oldHandlersP->chld);
#endif
}    



void
restoreSignalHandlers(struct signalHandlers const& oldHandlers) {

#ifndef _WIN32
    sigaction(SIGCHLD, &oldHandlers.chld, NULL);
    sigaction(SIGPIPE, &oldHandlers.pipe, NULL);
    sigaction(SIGUSR1, &oldHandlers.usr1, NULL);
    sigaction(SIGHUP,  &oldHandlers.hup,  NULL);
    sigaction(SIGINT,  &oldHandlers.int_, NULL);
    sigaction(SIGTERM, &oldHandlers.term, NULL);
#endif
}



// We need 'global' because methods of class serverAbyss call
// functions in the Abyss C library.  By virtue of global's static
// storage class, the program loader will call its constructor and
// destructor and thus initialize and terminate the Abyss C library.

class abyssGlobalState {
public:
    abyssGlobalState() {
        const char * error;
        AbyssInit(&error);
        if (error) {
            string const e(error);
            xmlrpc_strfree(error);
            throwf("AbyssInit() failed.  %s", e.c_str());
        }
    }
    ~abyssGlobalState() {
        AbyssTerm();
    }
} const global;

} // namespace



callInfo_serverAbyss::callInfo_serverAbyss(
    serverAbyss * const serverAbyssP,
    TSession *    const abyssSessionP) :
    serverAbyssP(serverAbyssP), abyssSessionP(abyssSessionP) {}



struct serverAbyss::constrOpt_impl {

    constrOpt_impl();

    struct value {
        xmlrpc_c::registryPtr      registryPtr;
        const xmlrpc_c::registry * registryP;
        XMLRPC_SOCKET  socketFd;
        unsigned int   portNumber;
        unsigned int   maxConn;
        unsigned int   maxConnBacklog;
        unsigned int   keepaliveTimeout;
        unsigned int   keepaliveMaxConn;
        unsigned int   timeout;
        bool           dontAdvertise;
        std::string    uriPath;
        bool           chunkResponse;
        std::string    allowOrigin;
        unsigned int   accessCtlMaxAge;
        const struct sockaddr * sockAddrP;
        socklen_t      sockAddrLen;
        std::string    logFileName;
        bool           serverOwnsSignals;
        bool           expectSigchld;
    } value;
    struct {
        bool registryPtr;
        bool registryP;
        bool socketFd;
        bool portNumber;
        bool maxConn;
        bool maxConnBacklog;
        bool keepaliveTimeout;
        bool keepaliveMaxConn;
        bool timeout;
        bool dontAdvertise;
        bool uriPath;
        bool chunkResponse;
        bool allowOrigin;
        bool accessCtlMaxAge;
        bool sockAddrP;
        bool sockAddrLen;
        bool logFileName;
        bool serverOwnsSignals;
        bool expectSigchld;
    } present;
};



serverAbyss::constrOpt_impl::constrOpt_impl() {
    present.registryPtr       = false;
    present.registryP         = false;
    present.socketFd          = false;
    present.portNumber        = false;
    present.logFileName       = false;
    present.maxConn           = false;
    present.maxConnBacklog    = false;
    present.keepaliveTimeout  = false;
    present.keepaliveMaxConn  = false;
    present.timeout           = false;
    present.dontAdvertise     = false;
    present.uriPath           = false;
    present.chunkResponse     = false;
    present.allowOrigin       = false;
    present.accessCtlMaxAge   = false;
    present.sockAddrP         = false;
    present.sockAddrLen       = false;
    present.serverOwnsSignals = false;
    present.expectSigchld     = false;
    
    // Set default values
    value.dontAdvertise     = false;
    value.uriPath           = string("/RPC2");
    value.chunkResponse     = false;
    value.serverOwnsSignals = true;
    value.expectSigchld     = false;
}



#define DEFINE_OPTION_SETTER(OPTION_NAME, TYPE) \
serverAbyss::constrOpt & \
serverAbyss::constrOpt::OPTION_NAME(TYPE const& arg) { \
    this->implP->value.OPTION_NAME = arg; \
    this->implP->present.OPTION_NAME = true; \
    return *this; \
}

DEFINE_OPTION_SETTER(registryPtr,       xmlrpc_c::registryPtr);
DEFINE_OPTION_SETTER(registryP,         const registry *);
DEFINE_OPTION_SETTER(socketFd,          XMLRPC_SOCKET);
DEFINE_OPTION_SETTER(portNumber,        unsigned int);
DEFINE_OPTION_SETTER(maxConn,           unsigned int);
DEFINE_OPTION_SETTER(maxConnBacklog,    unsigned int);
DEFINE_OPTION_SETTER(keepaliveTimeout,  unsigned int);
DEFINE_OPTION_SETTER(keepaliveMaxConn,  unsigned int);
DEFINE_OPTION_SETTER(timeout,           unsigned int);
DEFINE_OPTION_SETTER(dontAdvertise,     bool);
DEFINE_OPTION_SETTER(uriPath,           string);
DEFINE_OPTION_SETTER(chunkResponse,     bool);
DEFINE_OPTION_SETTER(allowOrigin,       string);
DEFINE_OPTION_SETTER(accessCtlMaxAge,   unsigned int);
DEFINE_OPTION_SETTER(sockAddrP,         const struct sockaddr *);
DEFINE_OPTION_SETTER(sockAddrLen,       socklen_t);
DEFINE_OPTION_SETTER(logFileName,       string);
DEFINE_OPTION_SETTER(serverOwnsSignals, bool);
DEFINE_OPTION_SETTER(expectSigchld,     bool);

#undef DEFINE_OPTION_SETTER


serverAbyss::constrOpt::constrOpt() {

    this->implP = new serverAbyss::constrOpt_impl();
}



serverAbyss::constrOpt::~constrOpt() {

    delete(this->implP);
}



struct SockAddr {

    const struct sockaddr * const sockAddrP;
    socklen_t               const sockAddrLen;

    SockAddr(const struct sockaddr * const sockAddrP,
             socklen_t               const sockAddrLen) :
        sockAddrP   (sockAddrP),
        sockAddrLen (sockAddrLen)
    {}
};



struct serverAbyss_impl {
    registryPtr regPtr;
        // This just holds a reference to the registry so that it may
        // get destroyed when the serverAbyss gets destroyed.  If the
        // creator of the serverAbyss is managing lifetime himself,
        // this is a null pointer.  'registryP' is what you really use
        // to access the registry.
    
    const registry * registryP;

    TServer cServer;

    TChanSwitch * chanSwitchP;
        // Handle of the channel switch we created.  Null if we didn't.
        // When user wants us to accept connections, we create a channel
        // switch and create a server based on it; otherwise, we don't.

    serverAbyss_impl(serverAbyss::constrOpt_impl const& opt,
                     serverAbyss *               const serverAbyssP);

    ~serverAbyss_impl();

    void
    run();

    void
    processCall(std::string   const& call,
                TSession *    const  abyssSessionP,
                std::string * const  responseP);

    serverAbyss * const serverAbyssP;
        // The server for which we are the implementation.

    bool expectSigchld;
    bool serverOwnsSignals;
};



static void
processXmlrpcCall(xmlrpc_env *        const envP,
                  void *              const arg,
                  const char *        const callXml,
                  size_t              const callXmlLen,
                  TSession *          const abyssSessionP,                  
                  xmlrpc_mem_block ** const responseXmlPP) {
/*----------------------------------------------------------------------------
   This is an XML-RPC XML call processor, as called by the HTTP request
   handler of the libxmlrpc_server_abyss C library.

   'callXml'/'callXmlLen' is the XML text of a supposed XML-RPC call.
   We execute the RPC and return the XML text of the XML-RPC response
   as *responseXmlPP.

   'arg' carries the information that tells us how to do that; e.g.
   what XML-RPC methods are defined.
-----------------------------------------------------------------------------*/
    serverAbyss_impl * const implP(
        static_cast<serverAbyss_impl *>(arg));

    try {
        string const call(callXml, callXmlLen);

        string response;

        implP->processCall(call, abyssSessionP, &response);

        xmlrpc_mem_block * responseMbP;

        responseMbP = XMLRPC_MEMBLOCK_NEW(char, envP, 0);

        if (!envP->fault_occurred) {
            XMLRPC_MEMBLOCK_APPEND(char, envP, responseMbP,
                                   response.c_str(), response.length());

            *responseXmlPP = responseMbP;
        }
    } catch (exception const& e) {
        xmlrpc_env_set_fault(envP, XMLRPC_INTERNAL_ERROR, e.what());
    }
}



static void
validateListenOptions(serverAbyss::constrOpt_impl const& opt) {
    
    if ((opt.present.portNumber ? 1 : 0) +
        (opt.present.socketFd ? 1 : 0) +
        (opt.present.sockAddrP ? 1 : 0) > 1)
        throwf("You can specify at most one of portNumber, socketFd, "
               "and sockAddrP options");

    if (opt.present.sockAddrP && !opt.present.sockAddrLen)
        throwf("You must specify the sockAddrLen option when you "
               "specify sockAddrP");

    if (!opt.present.sockAddrP && opt.present.sockAddrLen)
        throwf("The sockAddrLen option does not make sense without "
               "sockAddrP");

    if (opt.present.portNumber) {
        if (opt.value.portNumber > 0xffff)
            throwf("Port number %u exceeds the maximum possible port number "
                   "(65535)", opt.value.portNumber);
    }
}



static void
createServerFromSwitch(TServer *     const serverP,
                       TChanSwitch * const chanSwitchP) {

    const char * error;

    ServerCreateSwitch(serverP, chanSwitchP, &error);

    if (error) {
        throwf("Abyss failed to create server.  %s", error);
        xmlrpc_strfree(error);
    }
}



static TChanSwitch *
newChanSwitchOsSocket(int const socketFd) {

    TChanSwitch * chanSwitchP;
    const char * error;

#ifdef WIN32
    ChanSwitchWinCreateWinsock(socketFd, &chanSwitchP, &error);
#else
    ChanSwitchUnixCreateFd(socketFd, &chanSwitchP, &error);
#endif

    if (error) {
        string const errorS(error);
        xmlrpc_strfree(error);

        throwf("Abyss failed to create a channel switch from the "
               "supplied listen socket.  %s", errorS.c_str());
    }
    return chanSwitchP;
}



static void
chanSwitchCreateSockAddr(int                     const protocolFamily,
                         const struct sockaddr * const sockAddrP,
                         socklen_t               const sockAddrLen,
                         TChanSwitch **          const chanSwitchPP) {

    const char * error;

#ifdef WIN32
    ChanSwitchWinCreate2(protocolFamily, sockAddrP, sockAddrLen, 
                          chanSwitchPP, &error);
#else
    ChanSwitchUnixCreate2(protocolFamily, sockAddrP, sockAddrLen, 
                          chanSwitchPP, &error);
#endif
    if (error) {
        string const errorS(error);
        xmlrpc_strfree(error);
        throwf("Unable to create Abyss channel switch from socket address.  "
               "%s", errorS.c_str());
    }
}



static TChanSwitch *
newChanSwitchSockAddr(SockAddr const& sockAddr) {
    
    int protocolFamily;

    switch (sockAddr.sockAddrP->sa_family) {
    case AF_INET:
        protocolFamily = PF_INET;
        break;
    case AF_INET6:
        protocolFamily = PF_INET6;
        break;
    default:
        throwf("Unknown socket address family %d.  "
               "We know only AF_INET and AF_INET6.",
               sockAddr.sockAddrP->sa_family);
    }

    TChanSwitch * chanSwitchP;

    chanSwitchCreateSockAddr(protocolFamily,
                             sockAddr.sockAddrP, sockAddr.sockAddrLen,
                             &chanSwitchP);

    return chanSwitchP;
}



static TChanSwitch *
newChanSwitchIpV4Port(unsigned int const portNumber) {
    
    struct sockaddr_in sockAddr;

    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_port        = htons(portNumber);
    sockAddr.sin_addr.s_addr = INADDR_ANY;

    TChanSwitch * chanSwitchP;

    chanSwitchCreateSockAddr(PF_INET, (const struct sockaddr *)&sockAddr,
                             sizeof(sockAddr),
                             &chanSwitchP);

    return chanSwitchP;
}



static void
createServerBare(bool           const  logFileNameGiven,
                 string         const& logFileName,
                 bool           const  socketFdGiven,
                 int            const  socketFd,
                 bool           const  portNumberGiven,
                 unsigned int   const  portNumber,
                 bool           const  sockAddrPGiven,
                 SockAddr       const& sockAddr,
                 TServer *      const  serverP,
                 TChanSwitch ** const  chanSwitchPP) {

    const char * const serverName("XmlRpcServer");

    if (socketFdGiven || sockAddrPGiven || portNumberGiven) {

        TChanSwitch * const chanSwitchP(
            socketFdGiven ?
                newChanSwitchOsSocket(socketFd) : 
            sockAddrPGiven ? 
                newChanSwitchSockAddr(sockAddr) :
            portNumberGiven ?
                newChanSwitchIpV4Port(portNumber) :
                NULL);

        assert(chanSwitchP);

        try {
            createServerFromSwitch(serverP, chanSwitchP);

            try {
                ServerSetName(serverP, serverName);

                if (logFileNameGiven)
                    ServerSetLogFileName(serverP, logFileName.c_str());
            } catch (...) {
                ServerFree(serverP);
                throw;
            }
        } catch (...) {
            ChanSwitchDestroy(chanSwitchP);
            throw;
        }
        *chanSwitchPP = chanSwitchP;
    } else {
        const char * const logfileArg(logFileNameGiven ? 
                                      logFileName.c_str() : NULL);
        
        ServerCreateNoAccept(serverP, serverName,
                             DEFAULT_DOCS, logfileArg);

        *chanSwitchPP = NULL;
    }
}



static void
setAdditionalServerParms(TServer *                   const  serverP,
                         serverAbyss::constrOpt_impl const& opt) {

    if (opt.present.maxConn)
        ServerSetMaxConn(serverP, opt.value.maxConn);
    if (opt.present.maxConnBacklog)
        ServerSetMaxConnBacklog(serverP, opt.value.maxConnBacklog);
    if (opt.present.keepaliveTimeout)
        ServerSetKeepaliveTimeout(serverP, opt.value.keepaliveTimeout);
    if (opt.present.keepaliveMaxConn)
        ServerSetKeepaliveMaxConn(serverP, opt.value.keepaliveMaxConn);
    if (opt.present.timeout)
        ServerSetTimeout(serverP, opt.value.timeout);
    ServerSetAdvertise(serverP, !opt.value.dontAdvertise);
    if (opt.value.expectSigchld)
        ServerUseSigchld(serverP);
}



static void
setHttpReqHandlers(TServer *    const  serverP,
                   void *       const  serverHandle,
                   size_t       const  maxStackSize,
                   string       const& uriPath,
                   bool         const  chunkResponse,
                   bool         const  doHttpAccessControl,
                   string       const& allowOrigin,
                   bool         const  accessCtlExpires,
                   unsigned int const  accessCtlMaxAge) {

    env_wrap env;
    xmlrpc_server_abyss_handler_parms parms;

    parms.xml_processor = &processXmlrpcCall;
    parms.xml_processor_arg = serverHandle;
    parms.xml_processor_max_stack = maxStackSize;
    parms.uri_path = uriPath.c_str();
    parms.chunk_response = chunkResponse;
    parms.allow_origin = doHttpAccessControl ? allowOrigin.c_str() : NULL;
    parms.access_ctl_expires = accessCtlExpires;
    parms.access_ctl_max_age = accessCtlMaxAge;

    xmlrpc_server_abyss_set_handler3(
        &env.env_c, serverP,
        &parms, XMLRPC_AHPSIZE(access_ctl_max_age));
    
    if (env.env_c.fault_occurred)
        throwf("Failed to register the HTTP handler for XML-RPC "
               "with the underlying Abyss HTTP server.  "
               "xmlrpc_server_abyss_set_handler3() failed with:  %s",
               env.env_c.fault_string);

    xmlrpc_server_abyss_set_default_handler(serverP);
}
        


static void
createServer(serverAbyss::constrOpt_impl const& opt,
             void *                      const  serverHandle,
             size_t                      const  maxStackSize,
             TServer *                   const  serverP,
             TChanSwitch **              const  chanSwitchPP) {

    validateListenOptions(opt);
    
    createServerBare(opt.present.logFileName, opt.value.logFileName,
                     opt.present.socketFd,    opt.value.socketFd,
                     opt.present.portNumber,  opt.value.portNumber,
                     opt.present.sockAddrP,
                     SockAddr(opt.value.sockAddrP, opt.value.sockAddrLen),
                     serverP, chanSwitchPP);
    
    try {
        setAdditionalServerParms(serverP, opt);

        setHttpReqHandlers(serverP,
                           serverHandle,
                           maxStackSize,
                           opt.value.uriPath,
                           opt.value.chunkResponse,
                           opt.present.allowOrigin,
                           opt.value.allowOrigin,
                           opt.present.accessCtlMaxAge,
                           opt.value.accessCtlMaxAge);

        if (opt.present.portNumber || opt.present.socketFd ||
            opt.present.sockAddrP)
            ServerInit(serverP);
    } catch (...) {
        ServerFree(serverP);
        throw;
    }
}



serverAbyss_impl::serverAbyss_impl(
    serverAbyss::constrOpt_impl const& opt,
    serverAbyss *               const  serverAbyssP) :
    serverAbyssP(serverAbyssP) {

    if (!opt.present.registryP && !opt.present.registryPtr)
        throwf("You must specify the 'registryP' or 'registryPtr' option");
    else if (opt.present.registryP && opt.present.registryPtr)
        throwf("You may not specify both the 'registryP' and "
               "the 'registryPtr' options");
    else {
        if (opt.present.registryP)
            this->registryP = opt.value.registryP;
        else {
            this->regPtr = opt.value.registryPtr;
            this->registryP = this->regPtr.get();
        }
    }

    this->serverOwnsSignals = opt.value.serverOwnsSignals;
    
    if (opt.value.serverOwnsSignals && opt.value.expectSigchld)
        throwf("You can't specify both expectSigchld "
               "and serverOwnsSignals options");

    DateInit();

    createServer(opt, this, this->registryP->maxStackSize(),
                 &this->cServer, &this->chanSwitchP);
}



serverAbyss_impl::~serverAbyss_impl() {

    ServerFree(&this->cServer);

    if (this->chanSwitchP)
        ChanSwitchDestroy(this->chanSwitchP);
}



static void
setupSignalsAndRunAbyss(TServer * const abyssServerP) {

    /* We do some pretty ugly stuff for an object method: we set signal
       handlers, which are process-global.

       One example of where this can be hairy is: Caller has a child
       process unrelated to the Abyss server.  That child dies.  We
       get his death of a child signal and Caller never knows.

       We really expect to be the only thing in the process, at least
       for the time we're running.  If you want the Abyss Server
       to behave more like an object and own the signals yourself,
       use runOnce() in a loop instead of run().
    */
    signalHandlers oldHandlers;

    setupSignalHandlers(&oldHandlers);

    ServerUseSigchld(abyssServerP);

    ServerRun(abyssServerP);

    restoreSignalHandlers(oldHandlers);
}



void
serverAbyss_impl::run() {

    if (this->serverOwnsSignals)
        setupSignalsAndRunAbyss(&this->cServer);
    else {
        if (this->expectSigchld)
            ServerUseSigchld(&this->cServer);

        ServerRun(&this->cServer);
    }
}



void
serverAbyss_impl::processCall(string     const& call,
                              TSession * const  abyssSessionP,
                              string *   const  responseP) {

    callInfo_serverAbyss const callInfo(this->serverAbyssP, abyssSessionP);

    this->registryP->processCall(call, &callInfo, responseP);
}



serverAbyss::shutdown::shutdown(serverAbyss * const serverAbyssP) :
    serverAbyssP(serverAbyssP) {}



serverAbyss::shutdown::~shutdown() {}



void
serverAbyss::shutdown::doit(string const&,
                            void * const) const {

    this->serverAbyssP->terminate();
}



void
serverAbyss::initialize(constrOpt const& opt) {

    this->implP = new serverAbyss_impl(*opt.implP, this);
}



serverAbyss::serverAbyss(constrOpt const& opt) {

    initialize(opt);
}



serverAbyss::serverAbyss(
    xmlrpc_c::registry const& registry,
    unsigned int       const  portNumber,
    string             const& logFileName,
    unsigned int       const  keepaliveTimeout,
    unsigned int       const  keepaliveMaxConn,
    unsigned int       const  timeout,
    bool               const  dontAdvertise,
    bool               const  socketBound,
    XMLRPC_SOCKET      const  socketFd) {
/*----------------------------------------------------------------------------
  This is a backward compatibility interface.  This used to be the only
  constructor.
-----------------------------------------------------------------------------*/
    serverAbyss::constrOpt opt;

    opt.registryP(&registry);
    if (logFileName.length() > 0)
        opt.logFileName(logFileName);
    if (keepaliveTimeout > 0)
        opt.keepaliveTimeout(keepaliveTimeout);
    if (keepaliveMaxConn > 0)
        opt.keepaliveMaxConn(keepaliveMaxConn);
    if (timeout > 0)
        opt.timeout(timeout);
    opt.dontAdvertise(dontAdvertise);
    if (socketBound)
        opt.socketFd(socketFd);
    else
        opt.portNumber(portNumber);

    initialize(opt);
}



serverAbyss::~serverAbyss() {

    delete(this->implP);
}



void
serverAbyss::run() {

    this->implP->run();
}
 


void
serverAbyss::runOnce() {

    ServerRunOnce(&this->implP->cServer);
}



void
serverAbyss::runConn(int const socketFd) {

    ServerRunConn(&this->implP->cServer, socketFd);
}



#ifndef WIN32
void
serverAbyss::sigchld(pid_t const pid) {

    // There's a hole in the design here, because the Abyss server uses
    // a process-global list of children (so there can't be more than one
    // Abyss object in the process), so while this is an object method,
    // it doesn't really refer to the object at all.

    // We might conceivably fix Abyss some day, then this method would do
    // what you expect -- affect only its own object.  But forking Abyss is
    // obsolete anyway, so we just don't worry about it.

    ServerHandleSigchld(pid);
}
#endif



void
serverAbyss::terminate() {

    ServerTerminate(&this->implP->cServer);
}



callInfo_abyss::callInfo_abyss(TSession * const abyssSessionP) :
    abyssSessionP(abyssSessionP) {}



void
processXmlrpcCall2(xmlrpc_env *        const envP,
                   void *              const arg,
                   const char *        const callXml,
                   size_t              const callXmlLen,
                   TSession *          const abyssSessionP,                  
                   xmlrpc_mem_block ** const responseXmlPP) {
/*----------------------------------------------------------------------------
   This is an XML-RPC XML call processor, as called by the HTTP request
   handler of the libxmlrpc_server_abyss C library.

   'callXml'/'callXmlLen' is the XML text of a supposed XML-RPC call.
   We execute the RPC and return the XML text of the XML-RPC response
   as *responseXmlPP.

   'arg' carries the information that tells us how to do that; e.g.
   what XML-RPC methods are defined.
-----------------------------------------------------------------------------*/
    const registry * const registryP(static_cast<registry *>(arg));

    try {
        string const call(callXml, callXmlLen);
        callInfo_abyss const callInfo(abyssSessionP);

        string response;

        registryP->processCall(call, &callInfo, &response);

        xmlrpc_mem_block * responseMbP;

        responseMbP = XMLRPC_MEMBLOCK_NEW(char, envP, response.length());

        if (!envP->fault_occurred) {
            XMLRPC_MEMBLOCK_APPEND(char, envP, responseMbP,
                                   response.c_str(), response.length());

            *responseXmlPP = responseMbP;
        }
    } catch (exception const& e) {
        xmlrpc_env_set_fault(envP, XMLRPC_INTERNAL_ERROR, e.what());
    }
}



static void
setHandlers(TServer * const  serverP,
            string    const& uriPath,
            registry  const& registry) {

    xmlrpc_server_abyss_set_handler2(
        serverP, uriPath.c_str(),
        processXmlrpcCall2,
        const_cast<xmlrpc_c::registry *>(&registry),
        registry.maxStackSize(),
        false);

    xmlrpc_server_abyss_set_default_handler(serverP);
}



void
server_abyss_set_handlers(TServer * const  serverP,
                          registry  const& registry,
                          string    const& uriPath) {

    setHandlers(serverP, uriPath, registry);
}



void
server_abyss_set_handlers(TServer *        const  serverP,
                          const registry * const  registryP,
                          string           const& uriPath) {

    setHandlers(serverP, uriPath, *registryP);
}



void
server_abyss_set_handlers(TServer *   const  serverP,
                          registryPtr const  registryPtr,
                          string      const& uriPath) {

    setHandlers(serverP, uriPath, *registryPtr.get());
}



} // namespace
