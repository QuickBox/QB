/*=============================================================================
                                  server_abyss
===============================================================================
  Test the Abyss XML-RPC server C++ facilities of XML-RPC for C/C++.
  
=============================================================================*/
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/abyss.h */

#include <errno.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <time.h>
#ifdef WIN32
  #include <winsock2.h>
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <netinet/in.h>
#endif

using namespace std;

#include "xmlrpc-c/girerr.hpp"
using girerr::error;
using girerr::throwf;
#include "xmlrpc-c/base.hpp"
#include "xmlrpc-c/registry.hpp"
#include "xmlrpc-c/server_abyss.hpp"
#include "xmlrpc-c/abyss.h"

using namespace xmlrpc_c;

#include "tools.hpp"
#include "server_abyss.hpp"



namespace {

static void
closesock(int const fd) {
#ifdef WIN32
  closesocket(fd);
#else
  close(fd);
#endif
}



static sockaddr_in
localhostSockAddr(short const portNumber) {

    struct sockaddr_in retval;

    retval.sin_family = AF_INET;
    retval.sin_port = htons(portNumber);
    retval.sin_addr = test_ipAddrFromDecimal(127, 0, 0, 1);

    return retval;
}



class boundSocket {

public:
    boundSocket(short const portNumber) {
        this->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->fd < 0)
            throwf("socket() failed.  errno=%d (%s)",
                   errno, strerror(errno));
        
        struct sockaddr_in sockAddr;
        int rc;

        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port   = htons(portNumber);
        sockAddr.sin_addr.s_addr = 0;

        rc = bind(this->fd, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
        
        if (rc != 0) {
            closesock(this->fd);
            throwf("Couldn't bind.  bind() failed with errno=%d (%s)",
                   errno, strerror(errno));
        }
    }

    ~boundSocket() {
        closesock(this->fd);
    }

    XMLRPC_SOCKET fd;
};



class sampleAddMethod : public method {
public:
    sampleAddMethod() {
        this->_signature = "i:ii";
        this->_help = "This method adds two integers together";
    }
    void
    execute(xmlrpc_c::paramList const& paramList,
            value *             const  retvalP) {
        
        int const addend(paramList.getInt(0));
        int const adder(paramList.getInt(1));
        
        paramList.verifyEnd(2);
        
        *retvalP = value_int(addend + adder);
    }
};



// We need 'global' because methods of class addHandlerTestSuite call
// functions in the Abyss C library.  By virtue of global's static
// storage class, the program loader will call its constructor and
// destructor and thus initialize and terminate the Abyss C library.

static class abyssGlobalState {
public:
    abyssGlobalState() {
        const char * error;
        AbyssInit(&error);
        if (error) {
            string const e(error);
            free(const_cast<char *>(error));
            throwf("AbyssInit() failed.  %s", e.c_str());
        }
    }
    ~abyssGlobalState() {
        AbyssTerm();
    }
} const global;



class addHandlerTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "addHandlerTestSuite";
    }
    virtual void runtests(unsigned int const) {
        TServer abyssServer;

        ServerCreate(&abyssServer, "testserver", 8080, NULL, NULL);

        registry myRegistry;
        
        myRegistry.addMethod("sample.add", methodPtr(new sampleAddMethod));
        
        registryPtr myRegistryP(new registry);
        
        myRegistryP->addMethod("sample.add", methodPtr(new sampleAddMethod));

        server_abyss_set_handlers(&abyssServer, myRegistry);

        server_abyss_set_handlers(&abyssServer, &myRegistry);
        
        server_abyss_set_handlers(&abyssServer, myRegistryP);

        server_abyss_set_handlers(&abyssServer, myRegistry, "/RPC3");

        server_abyss_set_handlers(&abyssServer, &myRegistry, "/RPC3");
        
        server_abyss_set_handlers(&abyssServer, myRegistryP, "/RPC3");

        ServerFree(&abyssServer);
    }
};



class setShutdownTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "setShutdownTestSuite";
    }
    virtual void runtests(unsigned int const) {

        registry myRegistry;

        serverAbyss myServer(serverAbyss::constrOpt()
                             .registryP(&myRegistry)
                             .portNumber(12345)
            );

        serverAbyss::shutdown shutdown(&myServer);

        myRegistry.setShutdown(&shutdown);
    }
};



class createTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "createTestSuite";
    }
    virtual void runtests(unsigned int const) {
        
        registry myRegistry;
        
        myRegistry.addMethod("sample.add", methodPtr(new sampleAddMethod));

        registryPtr myRegistryP(new registry);
    
        myRegistryP->addMethod("sample.add", methodPtr(new sampleAddMethod));

        struct sockaddr_in const sockAddr(localhostSockAddr(8080));
        const struct sockaddr * const sockAddrP(
            (const struct sockaddr *)&sockAddr);

        EXPECT_ERROR(  // No registry
            serverAbyss::constrOpt opt;
            serverAbyss abyssServer(opt);
            );
        EXPECT_ERROR(  // Both registryP and registryPtr
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryPtr(myRegistryP)
                                    .registryP(&myRegistry)
                                    .portNumber(12345)
                );
            );
        EXPECT_ERROR(  // Both portNumber and socketFd
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .portNumber(8080)
                                    .socketFd(3));
            );
        
        EXPECT_ERROR(  // Both portNumber and sockAddrP
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .portNumber(8080)
                                    .sockAddrP(sockAddrP)
                                    .sockAddrLen(sizeof(sockAddr)));
            );
        
        EXPECT_ERROR(  // Both socketFd and sockAddrP
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .socketFd(3)
                                    .sockAddrP(sockAddrP)
                                    .sockAddrLen(sizeof(sockAddr)));
            );
        
        EXPECT_ERROR(  // sockAddrP but no sockAddrLen
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .sockAddrP(sockAddrP));
            );
    
        EXPECT_ERROR(  // port number too big
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .portNumber(65536));
            );
        
        // Because of the vagaries of Abyss, construction of the following
        // objects may exit the program if it detects an error, such as
        // port number already in use.  We need to fix Abyss some day.
    
        {
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .portNumber(12345)
                );
        }
        {
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryPtr(myRegistryP)
                                    .portNumber(12345)
                );
        }
        {
            boundSocket socket(12345);
            
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                                    .socketFd(socket.fd)
                );
        }
        {
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryP(&myRegistry)
                );
        }
    
        {
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryPtr(myRegistryP)
                                    .sockAddrP(sockAddrP)
                                    .sockAddrLen(sizeof(sockAddr))
                );
        }
        {
            // Test all the options
            serverAbyss abyssServer(serverAbyss::constrOpt()
                                    .registryPtr(myRegistryP)
                                    .portNumber(12345)
                                    .maxConn(10)
                                    .maxConnBacklog(10)
                                    .keepaliveTimeout(5)
                                    .keepaliveMaxConn(4)
                                    .timeout(20)
                                    .dontAdvertise(true)
                                    .uriPath("/xmlrpc")
                                    .chunkResponse(true)
                                    .allowOrigin("*")
                                    .accessCtlMaxAge(42)
                                    .logFileName("/tmp/logfile")
                                    .serverOwnsSignals(false)
                                    .expectSigchld(true)
                );
    
        }
        {
            serverAbyss abyssServer(
                myRegistry,
                12345,              // TCP port on which to listen
                "/tmp/xmlrpc_log"  // Log file
                );
        }
    }
};



class testCallInfoMethod : public method2 {
public:
    void
    execute(paramList        const& paramList,
            const callInfo * const  callInfoPtr,
            value *          const  retvalP) {

        const callInfo_serverAbyss * const callInfoP(
            dynamic_cast<const callInfo_serverAbyss *>(callInfoPtr));

        TEST(callInfoP != NULL);
        
        paramList.verifyEnd(0);

        TEST(callInfoP->serverAbyssP != NULL);
        TEST(callInfoP->abyssSessionP != NULL);
        
        *retvalP = value_nil();
    }
};



class callInfoTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "callInfoTestSuite";
    }
    virtual void runtests(unsigned int const) {
        
        registry myRegistry;
        
        myRegistry.addMethod("sample.add", methodPtr(new testCallInfoMethod));

        serverAbyss abyssServer(serverAbyss::constrOpt()
                                .registryP(&myRegistry)
                                .portNumber(12345)
            );
    }
};



} // unnamed namespace



string
serverAbyssTestSuite::suiteName() {
    return "serverAbyssTestSuite";
}


void
serverAbyssTestSuite::runtests(unsigned int const indentation) {

    addHandlerTestSuite().run(indentation+1);

    setShutdownTestSuite().run(indentation+1);

    createTestSuite().run(indentation+1);

    callInfoTestSuite().run(indentation+1);
}
