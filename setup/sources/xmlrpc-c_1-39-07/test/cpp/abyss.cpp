/*=============================================================================
                                  abyss
===============================================================================
  Test the Abyss server C++ facilities of XML-RPC for C/C++.
  
=============================================================================*/
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/abyss.h */

#include <string>

using namespace std;

#include "xmlrpc-c/girerr.hpp"
using girerr::error;
using girerr::throwf;
#include "xmlrpc-c/base.hpp"
#include "xmlrpc-c/AbyssServer.hpp"
#include "xmlrpc-c/AbyssEnvironment.hpp"
#include "xmlrpc-c/AbyssChanSwitch.hpp"

using namespace xmlrpc_c;

#include "tools.hpp"
#include "abyss.hpp"



// We need 'global' because methods of class addHandlerTestSuite call
// functions in the Abyss C library.  By virtue of global's static
// storage class, the program loader will call its constructor and
// destructor and thus initialize and terminate the Abyss C library.

static const AbyssEnvironment global;

namespace {



class addHandlerTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "addHandlerTestSuite";
    }
    virtual void runtests(unsigned int const) {

        class AbyssReqHandler_Test : public AbyssServer::ReqHandler {
        public:
            AbyssReqHandler_Test() {}

            void
            handleRequest(AbyssServer::Session * const ,
                          bool *                 const ) {}
        };

        AbyssChanSwitchUnix chanSwitch(8080);

        AbyssServer abyssServer(&chanSwitch);

        AbyssReqHandler_Test abyssReqHandler;

        abyssServer.addRequestHandler(&abyssReqHandler);
    }
};



class createTestSuite : public testSuite {

public:
    virtual string suiteName() {
        return "createTestSuite";
    }
    virtual void runtests(unsigned int const) {
        
        AbyssChanSwitchUnix chanSwitch(8080);

        AbyssServer abyssServer(&chanSwitch);

    }
};



}  // namespace



string
abyssTestSuite::suiteName() {
    return "abyssTestSuite";
}


void
abyssTestSuite::runtests(unsigned int const indentation) {

    createTestSuite().run(indentation+1);

    addHandlerTestSuite().run(indentation+1);
}



