#include <string>

using namespace std;

#include <xmlrpc-c/girerr.hpp>
using girerr::throwf;
#include <xmlrpc-c/string_int.h>
#include <xmlrpc-c/abyss.h>

#include "xmlrpc-c/AbyssEnvironment.hpp"



namespace xmlrpc_c {



AbyssEnvironment::AbyssEnvironment() {

    const char * error;

    AbyssInit(&error);

    if (error) {
        string const errorStr(error);

        xmlrpc_strfree(error);

        throwf("AbyssInit() failed.  %s", errorStr.c_str());
    }
}



AbyssEnvironment::~AbyssEnvironment() {

    AbyssTerm();
}



}  // namespace
