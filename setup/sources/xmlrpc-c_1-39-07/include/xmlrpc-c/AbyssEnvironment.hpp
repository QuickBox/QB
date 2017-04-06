/*============================================================================
                              AbyssEnvironment.hpp
==============================================================================
  This declares class AbyssEnvironment, which is an essential part of a
  program that uses other classes in libxmlrpc_abyss++.
============================================================================*/
#ifndef ABYSS_ENVIRONMENT_HPP_INCLUDED
#define ABYSS_ENVIRONMENT_HPP_INCLUDED

namespace xmlrpc_c {

class AbyssEnvironment {
/*----------------------------------------------------------------------------
   A program that uses libxmlrpc_abyss++ should contain exactly one object
   of this class, as a global static object.

   This is necessary because the library depends upon other libraries that
   have global variables.
-----------------------------------------------------------------------------*/
public:
    AbyssEnvironment();

    ~AbyssEnvironment();
};
}  // namespace
#endif
