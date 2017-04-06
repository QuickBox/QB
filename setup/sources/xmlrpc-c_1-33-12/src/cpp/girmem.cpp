#include <cassert>

#include "xmlrpc-c/girerr.hpp"
using girerr::error;
#include "Lock.hpp"

#include "xmlrpc-c/girmem.hpp"

using namespace std;
using namespace xmlrpc_c;


namespace girmem {


class autoObject::Impl {

    Lock refcountLock;
    unsigned int refcount;

public:
    Impl();

    ~Impl();

    void
    incref();

    void
    decref(bool * const unreferencedP);
};



autoObject::Impl::Impl() {

    this->refcount = 0;
}



autoObject::Impl::~Impl() {

    if (this->refcount > 0)
        throw(error("Destroying referenced object"));
}



void
autoObject::Impl::incref() {

    Lock::Holder(&this->refcountLock);

    ++this->refcount;
}



void
autoObject::Impl::decref(bool * const unreferencedP) {

    if (this->refcount == 0)
        throw(error("Decrementing ref count of unreferenced object"));

    Lock::Holder(&this->refcountLock);

    --this->refcount;
    *unreferencedP = (this->refcount == 0);
}
 


autoObject::autoObject() : implP(auto_ptr<Impl>(new Impl)) {}



autoObject::autoObject(autoObject const&) {
    // This method is declared private, so we can be running now:
    assert(false);
}



autoObject::~autoObject() {}



void
autoObject::incref() {
    this->implP->incref();
}



void
autoObject::decref(bool * const unreferencedP) {
    this->implP->decref(unreferencedP);
}



autoObjectPtr::autoObjectPtr() : objectP(NULL) {}



autoObjectPtr::autoObjectPtr(autoObject * const objectP) {

    // Note: When someone attempts to use this constructor with a null
    // argument, it's normally because a 'new' of the autoObject
    // failed, before calling the autoObject's constructor, thus
    // generating a null pointer.

    // E.g. the following code, where the system is out of memory:
    //
    //    class client    : public autoObject    { ... }
    //    class clientPtr : public autoObjectPtr { ... }
    //    clientPtr clientP(new client);

    if (objectP == NULL)
        throw(error("Object creation failed; trying to create autoObjectPtr "
                    "with a null autoObject pointer"));
        
    this->objectP = objectP;
    objectP->incref();
}



autoObjectPtr::autoObjectPtr(autoObjectPtr const& autoObjectPtr) {
    // copy constructor

    this->objectP = autoObjectPtr.objectP;
    if (this->objectP)
        this->objectP->incref();
}
    
 

autoObjectPtr::~autoObjectPtr() {

    this->unpoint();
}


 
void
autoObjectPtr::point(autoObject * const objectP) {

    if (this->objectP != NULL)
        throw(error("Already pointing"));
    this->objectP = objectP;
    objectP->incref();
}



void
autoObjectPtr::unpoint() {

    if (this->objectP) {
        bool dead;
        this->objectP->decref(&dead);
        if (dead) {
            delete(this->objectP);
            this->objectP = NULL;
        }
    }
}



autoObjectPtr
autoObjectPtr::operator=(autoObjectPtr const& source) {

    // If we're overwriting a variable that already points to something,
    // we have to unpoint it from what it points to now before we can point
    // it to what 'source' points to.  But if the source and destination
    // are the same object, we just want to leave the pointing alone.

    if (this == &source) {
        // Assignment of variable to itself; no-op
    } else {
        this->unpoint();
        this->objectP = source.objectP;
        if (this->objectP)
            this->objectP->incref();
    }
    return *this;
}

   

autoObject *
autoObjectPtr::operator->() const {
    if (this->objectP == NULL)
        throw(error("attempt to dereference autoObjectPtr "
                    "which does not point to anything"));
    return this->objectP;
}



autoObject *
autoObjectPtr::get() const {

    return this->objectP;
}

} // namespace
