/* This code is just a first draft by someone who doesn't know anything
   about Windows.  It has never been compiled.  It is just a framework
   for someone who knows Windows to pick and finish.

   Bryan Henderson redesigned the threading structure for Abyss in
   April 2006 and wrote this file then.  He use the former
   threading code, which did work on Windows, as a basis.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <winsock2.h>

#include "xmlrpc_config.h"

#include "bool.h"
#include "int.h"
#include "xmlrpc-c/util_int.h"
#include "mallocvar.h"
#include "xmlrpc-c/string_int.h"

#include "xmlrpc-c/abyss.h"
#include "trace.h"

#include "thread.h"


struct abyss_thread {
    HANDLE handle;
    void * userHandle;
    TThreadProc *   func;
    TThreadDoneFn * threadDone;
};

#define  MIN_THREAD_STACK_SIZE (16*1024L)


typedef uint32_t (WINAPI WinThreadProc)(void *);


static WinThreadProc threadRun;

static uint32_t WINAPI
threadRun(void * const arg) {

    struct abyss_thread * const threadP = arg;

    threadP->func(threadP->userHandle);

    threadP->threadDone(threadP->userHandle);

    return 0;
}



void
ThreadCreate(TThread **      const threadPP,
             void *          const userHandle,
             TThreadProc   * const func,
             TThreadDoneFn * const threadDone,
             bool            const useSigchld,
             size_t          const stackSize,
             const char **   const errorP) {

    TThread * threadP;

    MALLOCVAR(threadP);

    if (threadP == NULL)
        xmlrpc_asprintf(errorP,
                        "Can't allocate memory for thread descriptor.");
    else {
        DWORD z;

        threadP->userHandle = userHandle;
        threadP->func       = func;
        threadP->threadDone = threadDone;

        threadP->handle = (HANDLE)
            _beginthreadex(NULL,
                           MAX(stackSize, MIN_THREAD_STACK_SIZE),
                           threadRun,
                           threadP,
                           CREATE_SUSPENDED,
                           &z);

        if (threadP->handle == NULL)
            xmlrpc_asprintf(errorP, "_beginthreadex() failed.");
        else {
            *errorP = NULL;
            *threadPP = threadP;
        }
        if (*errorP)
            free(threadP);
    }
}



bool
ThreadRun(TThread * const threadP) {
    return (ResumeThread(threadP->handle) != 0xFFFFFFFF);
}



bool
ThreadStop(TThread * const threadP) {

    return (SuspendThread(threadP->handle) != 0xFFFFFFFF);
}



bool
ThreadKill(TThread * const threadP) {
    return (TerminateThread(threadP->handle, 0) != 0);
}



void
ThreadWaitAndRelease(TThread * const threadP) {

    WaitForSingleObject(threadP->handle, INFINITE);
    ThreadRelease(threadP);
}



void
ThreadExit(TThread * const threadP,
           int       const retValue) {

    threadP->threadDone(threadP->userHandle);

    _endthreadex(retValue);
}



void
ThreadRelease(TThread * const threadP) {

    CloseHandle(threadP->handle);

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
 
 

