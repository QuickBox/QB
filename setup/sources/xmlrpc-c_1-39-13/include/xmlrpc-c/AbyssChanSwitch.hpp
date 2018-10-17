/*============================================================================
                              AbyssChanSwitch.hpp
==============================================================================

  This declares class AbyssChanSwitch, which provides communication facilities
  for use with an AbyssServer object.

============================================================================*/
#ifndef ABYSS_CHANSWITCH_HPP_INCLUDED
#define ABYSS_CHANSWITCH_HPP_INCLUDED

#include <xmlrpc-c/abyss.h>

namespace xmlrpc_c {

class AbyssChanSwitch {
/*----------------------------------------------------------------------------
   An object of this class is a channel switch for use with an AbyssServer
   object, which means it listens for and accepts requests from clients to
   open a communication channel to the server (in its most popular form,
   it is an interface to a Unix stream socket in listen mode).

   This is an abstract base class.  There are derived classes for specific
   kinds of underlying communication media, such as Unix sockets.
-----------------------------------------------------------------------------*/
public:
    ~AbyssChanSwitch();

    TChanSwitch *
    cChanSwitchP() const;

protected:
    AbyssChanSwitch();

    TChanSwitch * _cChanSwitchP;
        // NULL when derived class constructor has not yet created the
        // TChanSwitch object.
};

class AbyssChanSwitchUnix : public AbyssChanSwitch {

public:
    AbyssChanSwitchUnix(unsigned short const listenPortNum);
};

}  // namespace
#endif
