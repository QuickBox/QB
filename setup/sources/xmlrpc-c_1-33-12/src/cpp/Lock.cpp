#include <xmlrpc-c/lock.h>
#include <xmlrpc-c/lock_platform.h>
#include "xmlrpc-c/girerr.hpp"
using girerr::error;

#include "Lock.hpp"


namespace xmlrpc_c {

Lock::Holder::Holder(Lock * const lockP)
/*-----------------------------------------------------------------------------
   Construct a holder of lock *lockP -- as long as this object exists, *lockP
   is locked.

   If *lockP is already locked, wait for it to be unlocked.
-----------------------------------------------------------------------------*/
    :
    lockP(lockP) 
{
    lockP->acquire();
}



Lock::Holder::~Holder() {

    this->lockP->release();
}



Lock::Lock() :
    c_lockP(xmlrpc_lock_create()){

    if (this->c_lockP == NULL)
        throw(error("Failed to create lock.  xmlrpc_lock_create() failed"));
}



Lock::~Lock() {

    this->c_lockP->destroy(this->c_lockP);
}



void
Lock::acquire() {

    this->c_lockP->acquire(this->c_lockP);
}



void
Lock::release() {

    this->c_lockP->release(this->c_lockP);
}



}  // namespace
