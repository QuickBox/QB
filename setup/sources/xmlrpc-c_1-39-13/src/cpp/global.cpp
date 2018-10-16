#include <string>

using namespace std;

#include <xmlrpc-c/girerr.hpp>
using girerr::throwf;
#include <xmlrpc-c/base.h>


class LibxmlrpcGlobalState {

public:
    LibxmlrpcGlobalState() {
        xmlrpc_env env;
        xmlrpc_env_init(&env);
        xmlrpc_init(&env);
        if (env.fault_occurred) {
            string const fault(env.fault_string);
            xmlrpc_env_clean(&env);
            throwf("Failed to initailize libxmlrpc.  %s", fault.c_str());
        }
    }

    ~LibxmlrpcGlobalState() {
        xmlrpc_term();
    }
};

static LibxmlrpcGlobalState libxmlrpcGlobalState;

