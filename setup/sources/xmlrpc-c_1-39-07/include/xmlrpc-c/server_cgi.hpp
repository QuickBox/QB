#ifndef SERVER_CGI_HPP_INCLUDED
#define SERVER_CGI_HPP_INCLUDED

#include <xmlrpc-c/c_util.h>
#include <xmlrpc-c/registry.hpp>

namespace xmlrpc_c {

/*
  XMLRPC_SERVER_CGIPP_EXPORTED marks a symbol in this file that is exported
  from libxmlrpc_server_cgi++.

  XMLRPC_BUILDING_SERVER_CGIPP says this compilation is part of
  libxmlrpc_server_cgi++, as opposed to something that _uses_
  libxmlrpc_server_cgi++.
*/
#ifdef XMLRPC_BUILDING_SERVER_CGIPP
#define XMLRPC_SERVER_CGIPP_EXPORTED XMLRPC_DLLEXPORT
#else
#define XMLRPC_SERVER_CGIPP_EXPORTED
#endif

class XMLRPC_SERVER_CGIPP_EXPORTED serverCgi {

public:

    class XMLRPC_SERVER_CGIPP_EXPORTED constrOpt {
    public:
        constrOpt();

        constrOpt & registryPtr       (xmlrpc_c::registryPtr      const& arg);
        constrOpt & registryP         (const xmlrpc_c::registry * const& arg);

        struct value {
            xmlrpc_c::registryPtr      registryPtr;
            const xmlrpc_c::registry * registryP;
        } value;
        struct {
            bool registryPtr;
            bool registryP;
        } present;
    };

    serverCgi(constrOpt const& opt);

    ~serverCgi();

    void
    processCall();

private:

    struct serverCgi_impl * implP;
};


} // namespace

#endif
