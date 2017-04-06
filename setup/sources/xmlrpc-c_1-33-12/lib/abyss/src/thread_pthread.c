#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "xmlrpc_config.h"

#include "bool.h"
#include "mallocvar.h"
#include "xmlrpc-c/util_int.h"
#include "xmlrpc-c/string_int.h"
#include "pthreadx.h"

#include "xmlrpc-c/abyss.h"

#include "thread.h"



struct abyss_thread {
    pthread_t       thread;
    void *          userHandle;
    TThreadProc *   func;
    TThreadDoneFn * threadDone;
};

/* We used to have MIN_STACK_SIZE = 16K, which was said to be the
   minimum stack size on Win32.  Scott Kolodzeski found in November
   2005 that this was insufficient for 64 bit Solaris -- we fail
   when creating the first thread.  So we changed to 128K.
*/
#define  MIN_STACK_SIZE (128*1024L)


typedef void * (pthreadStartRoutine)(void *);



static pthreadStartRoutine execute;

static void *
execute(void * const arg) {

    /* We make sure the user's "thread done" function runs as the thread
       exits, whether it exits by directly calling ThreadExit() or by
       returning to caller.

       In the direct exit case, the OS calls the "thread done" function by
       virtual of a cleanup that we set up.  In the return to caller case, we
       get control back and our call to pthread_cleanup_pop() calls the
       "thread done" function.
    */

    struct abyss_thread * const threadP = arg;
    bool const executeTrue = true;

    pthread_cleanup_push(threadP->threadDone, threadP->userHandle);

    threadP->func(threadP->userHandle);

    pthread_cleanup_pop(executeTrue);

    return NULL;
}




void
ThreadCreate(TThread **      const threadPP,
             void *          const userHandle,
             TThreadProc   * const func,
             TThreadDoneFn * const threadDone,
             bool            const useSigchld ATTR_UNUSED,
             size_t          const stackSize,
             const char **   const errorP) {

    if ((size_t)(int)stackSize != stackSize)
        xmlrpc_asprintf(errorP, "Stack size %lu is too big",
                        (unsigned long)stackSize);
    else {
        TThread * threadP;

        MALLOCVAR(threadP);
        if (threadP == NULL)
            xmlrpc_asprintf(errorP,
                            "Can't allocate memory for thread descriptor.");
        else {
            pthread_attr_t attr;
            int rc;

            pthread_attr_init(&attr);

            pthread_attr_setstacksize(&attr, MAX(MIN_STACK_SIZE, stackSize));
        
            threadP->userHandle = userHandle;
            threadP->func       = func;
            threadP->threadDone = threadDone;

            rc = pthread_create(&threadP->thread, &attr,
                                execute, threadP);
            if (rc == 0) {
                *errorP = NULL;
                *threadPP = threadP;
            } else
                xmlrpc_asprintf(
                    errorP, "pthread_create() failed, errno = %d (%s)",
                    errno, strerror(errno));
        
            pthread_attr_destroy(&attr);

            if (*errorP)
                free(threadP);
        }
    }
}



bool
ThreadRun(TThread * const threadP ATTR_UNUSED) {
    return TRUE;    
}



bool
ThreadStop(TThread * const threadP ATTR_UNUSED) {
    return TRUE;
}



bool
ThreadKill(TThread * const threadP ATTR_UNUSED) {

    return (pthread_kill(threadP->thread, SIGTERM) == 0);
}



void
ThreadWaitAndRelease(TThread * const threadP) {

    void * threadReturn;

    pthread_join(threadP->thread, &threadReturn);

    free(threadP);
}



void
ThreadExit(TThread * const threadP ATTR_UNUSED,
           int       const retValue) {

    pthread_exit((void*)&retValue);

    /* Note that the above runs our cleanup routine (which we registered
       with pthread_cleanup_push() before exiting.
    */
}



void
ThreadRelease(TThread * const threadP) {

    pthread_detach(threadP->thread);

    free(threadP);
}



bool
ThreadForks(void) {

    return FALSE;
}



void
ThreadUpdateStatus(TThread * const threadP ATTR_UNUSED) {

    /* Threads keep their own statuses up to date, so there's nothing
       to do here.
    */
}



void
ThreadHandleSigchld(pid_t const pid ATTR_UNUSED) {

    /* Death of a child signals have nothing to do with pthreads */
}



