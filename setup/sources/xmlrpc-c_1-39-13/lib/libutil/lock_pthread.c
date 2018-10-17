#include <stdlib.h>
#include <pthread.h>

#include "mallocvar.h"

#include "xmlrpc-c/lock.h"

#include "xmlrpc-c/lock_pthread.h"

static lockAcquireFn acquire;

static void
acquire(struct lock * const lockP) {

    pthread_mutex_t * const mutexP = lockP->implementationP;

    pthread_mutex_lock(mutexP);
}



static lockReleaseFn release;

static void
release(struct lock * const lockP) {

    pthread_mutex_t * const mutexP = lockP->implementationP;

    pthread_mutex_unlock(mutexP);
}



static lockDestroyFn destroy;

static void
destroy(struct lock * const lockP) {

    pthread_mutex_t * const mutexP = lockP->implementationP;

    pthread_mutex_destroy(mutexP);

    free(mutexP);

    free(lockP);
}



struct lock *
xmlrpc_lock_create_pthread(void) {

    struct lock * lockP;

    MALLOCVAR(lockP);

    if (lockP) {
        pthread_mutex_t * mutexP;

        MALLOCVAR(mutexP);

        if (mutexP) {
            pthread_mutex_init(mutexP, NULL);

            lockP->implementationP = mutexP;
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



