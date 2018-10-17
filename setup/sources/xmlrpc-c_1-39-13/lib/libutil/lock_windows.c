#include "xmlrpc_config.h"

/* We define WIN32_WIN_LEAN_AND_MEAN to make <windows.h> contain less
   junk; nothing in Xmlrpc-c needs that stuff.  One significant thing it cuts
   out is <winsock.h>, which would conflict with the <winsock2.h> that our
   includer might use.
*/
#define WIN32_WIN_LEAN_AND_MEAN
#include <windows.h>

#include "mallocvar.h"

#include "xmlrpc-c/lock.h"

#include "xmlrpc-c/lock_windows.h"

static lockAcquireFn acquire;

static void
acquire(struct lock * const lockP) {

    CRITICAL_SECTION * const criticalSectionP = lockP->implementationP;

    EnterCriticalSection(criticalSectionP);
}



static lockReleaseFn release;

static void
release(struct lock * const lockP) {

    CRITICAL_SECTION * const criticalSectionP = lockP->implementationP;

    LeaveCriticalSection(criticalSectionP);
}



static lockDestroyFn destroy;

static void
destroy(struct lock * const lockP) {

    CRITICAL_SECTION * const criticalSectionP = lockP->implementationP;

    DeleteCriticalSection(criticalSectionP);

    free(criticalSectionP);

    free(lockP);
}



struct lock *
xmlrpc_lock_create_windows(void) {

    struct lock * lockP;

    MALLOCVAR(lockP);

    if (lockP) {
        CRITICAL_SECTION * criticalSectionP;

        MALLOCVAR(criticalSectionP);

        if (criticalSectionP) {
            InitializeCriticalSection(criticalSectionP);

            lockP->implementationP = criticalSectionP;
            lockP->acquire = &acquire;
            lockP->release = &release;
            lockP->destroy = &destroy;
        } else {
            free(lockP);
            lockP = NULL;
        }
    }
    return lockP;
}



