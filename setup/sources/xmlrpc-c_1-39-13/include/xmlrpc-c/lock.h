#ifndef LOCK_H_INCLUDED
#define LOCK_H_INCLUDED

typedef struct lock lock;

typedef void lockAcquireFn(lock *);
typedef void lockReleaseFn(lock *);
typedef void lockDestroyFn(lock *);

struct lock {
    /* To finish the job of making an abstract lock class that can use locks
       other than pthread mutexes, we need to replace 'theLock' with a
       "void * implementationP" and make curlLock_create_pthread() malloc
       the mutex.
    */
    void * implementationP;
    lockAcquireFn * acquire;
    lockReleaseFn * release;
    lockDestroyFn * destroy;
};

#endif
