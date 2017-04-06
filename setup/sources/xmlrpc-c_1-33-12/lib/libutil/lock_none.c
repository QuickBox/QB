/*=============================================================================
                                  lock_none
===============================================================================
  This is an inter-thread lock class for a system in which there is no
  inter-thread conflict, e.g. where no two threads every share memory.

  The locking is therefore trivial: no-op
=============================================================================*/
#include <stdlib.h>

#include "mallocvar.h"
#include "xmlrpc-c/lock.h"

#include "xmlrpc-c/lock_none.h"

static lockAcquireFn acquire;

static void
acquire(struct lock * const lockP ATTR_UNUSED) {

}



static lockReleaseFn release;

static void
release(struct lock * const lockP ATTR_UNUSED) {

}



static lockDestroyFn destroy;

static void
destroy(struct lock * const lockP ATTR_UNUSED) {

    free(lockP);
}



struct lock *
xmlrpc_lock_create_none(void) {

    struct lock * lockP;

    MALLOCVAR(lockP);

    if (lockP) {
        lockP->implementationP = NULL;
        lockP->acquire = &acquire;
        lockP->release = &release;
        lockP->destroy = &destroy;
    }
    return lockP;
}



