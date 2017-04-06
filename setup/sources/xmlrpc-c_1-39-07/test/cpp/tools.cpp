#include <string>
#include <sstream>
#include <iostream>
#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <netinet/in.h>
#endif

#include "xmlrpc-c/girerr.hpp"
using girerr::error;
using girerr::throwf;

#include "tools.hpp"

using namespace std;

testSuite::~testSuite() {
}



void
testSuite::run(unsigned int const indentation) {
    try {
        cout << string(indentation*2, ' ') 
             << "Running " << suiteName() << endl;
        this->runtests(indentation);
    } catch (error const& error) {
        throwf("%s failed.  %s", suiteName().c_str(), error.what());
    } catch (...) {
        throw(error(suiteName() + string(" failed.  ") +
                    string("It threw an unexpected type of object")));
    }
    cout << string(indentation*2, ' ') 
         << suiteName() << " tests passed." << endl;
}



// This is a good place to set a breakpoint.
void 
logFailedTest(const char * const fileName, 
              unsigned int const lineNum, 
              const char * const statement) {

    ostringstream msg;

    msg << endl
        << fileName << ":" << lineNum 
        << ": expected (" << statement << ")" << endl;

    throw(error(msg.str()));
}


error
fileLineError(string       const filename,
              unsigned int const lineNumber,
              string       const description) {
    
    ostringstream combined;
    
    combined << filename << ":" << lineNumber << " " << description;
    
    return error(combined.str());
}



struct in_addr
test_ipAddrFromDecimal(unsigned int const byte0,
                       unsigned int const byte1,
                       unsigned int const byte2,
                       unsigned int const byte3) {

    struct in_addr retval;

    retval.s_addr =
        htonl((byte0 << 24) + (byte1 << 16) + (byte2 << 8) + (byte3 << 0));

    return retval;
}



