/*=============================================================================
                                 socket_unix.c
===============================================================================
  This is the implementation of TChanSwitch and TChannel (and
  obsolete TSocket) for a standard Unix (POSIX)
  stream socket -- what you create with a socket() C library call.
=============================================================================*/

#include "xmlrpc_config.h"

#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#if HAVE_SYS_FILIO_H
  #include <sys/filio.h>
#endif

#include "c_util.h"
#include "int.h"
#include "girstring.h"
#include "xmlrpc-c/util_int.h"
#include "xmlrpc-c/string_int.h"
#include "mallocvar.h"
#include "trace.h"
#include "chanswitch.h"
#include "channel.h"
#include "socket.h"
#include "netinet/in.h"
#include "xmlrpc-c/abyss.h"

#include "socket_unix.h"



typedef struct {
    int interruptorFd;
    int interrupteeFd;
} interruptPipe;



static void
initInterruptPipe(interruptPipe * const pipeP,
                  const char **   const errorP) {

    int pipeFd[2];
    int rc;

    rc = pipe(pipeFd);

    if (rc != 0)
        xmlrpc_asprintf(errorP, "Unable to create a pipe to use to interrupt "
                        "waits.  pipe() failed with errno %d (%s)",
                        errno, strerror(errno));
    else {
        *errorP = NULL;
        pipeP->interruptorFd = pipeFd[1];
        pipeP->interrupteeFd = pipeFd[0];
    }
}



static void
termInterruptPipe(interruptPipe const pipe) {

    close(pipe.interruptorFd);
    close(pipe.interrupteeFd);
}



struct socketUnix {
/*----------------------------------------------------------------------------
   The properties/state of a TChanSwitch or TChannel unique to the
   Unix variety.
-----------------------------------------------------------------------------*/
    int fd;
        /* File descriptor of the POSIX socket (such as is created by
           socket() in the C library) for the socket.
        */
    bool userSuppliedFd;
        /* The file descriptor and associated POSIX socket belong to the
           user; we did not create it.
        */
    interruptPipe interruptPipe;
};



static bool
connected(int const fd) {
/*----------------------------------------------------------------------------
   Return TRUE iff the socket on file descriptor 'fd' is in the connected
   state.

   If 'fd' does not identify a stream socket or we are unable to determine
   the state of the stream socket, the answer is "false".
-----------------------------------------------------------------------------*/
    bool connected;
    struct sockaddr sockaddr;
    socklen_t nameLen;
    int rc;

    nameLen = sizeof(sockaddr);

    rc = getpeername(fd, &sockaddr, &nameLen);

    if (rc == 0)
        connected = true;
    else
        connected = false;

    return connected;
}



void
SocketUnixInit(const char ** const errorP) {

    *errorP = NULL;
}



void
SocketUnixTerm(void) {

}



/*=============================================================================
      TChannel
=============================================================================*/

static void
channelDestroy(TChannel * const channelP) {

    struct socketUnix * const socketUnixP = channelP->implP;

    termInterruptPipe(socketUnixP->interruptPipe);

    if (!socketUnixP->userSuppliedFd)
        close(socketUnixP->fd);

    free(socketUnixP);
}



static ChannelWriteImpl channelWrite;

static void
channelWrite(TChannel *            const channelP,
             const unsigned char * const buffer,
             uint32_t              const len,
             bool *                const failedP) {

    struct socketUnix * const socketUnixP = channelP->implP;

    size_t bytesLeft;
    bool error;

    assert(sizeof(size_t) >= sizeof(len));

    for (bytesLeft = len, error = false;
         bytesLeft > 0 && !error;
        ) {
        size_t const maxSend = (size_t)(-1) >> 1;

        ssize_t rc;

        // We'd like to use MSG_NOSIGNAL here, to prevent this send() from
        // causing a SIGPIPE if the other end of the socket is closed, but
        // MSG_NOSIGNAL is not standard enough.  An SO_NOSIGPIPE socket
        // option is another way, but even less standard.  So instead, the
        // thread simply must be set to ignore SIGPIPE.
        
        rc = send(socketUnixP->fd, &buffer[len-bytesLeft],
                  MIN(maxSend, bytesLeft), 0);

        if (ChannelTraceIsActive) {
            if (rc < 0)
                fprintf(stderr, "Abyss channel: send() failed.  errno=%d (%s)",
                        errno, strerror(errno));
            else if (rc == 0)
                fprintf(stderr, "Abyss channel: send() failed.  "
                        "Socket closed.\n");
            else {
                size_t const bytesTransferred = rc;
                fprintf(stderr, "Abyss channel: sent %u bytes: '%.*s'\n",
                        (unsigned)bytesTransferred,
                        (int)(MIN(bytesTransferred, 4096)),
                        &buffer[len-bytesLeft]);
            }
        }
        if (rc <= 0)
            /* 0 means connection closed; < 0 means severe error */
            error = true;
        else
            bytesLeft -= rc;
    }
    *failedP = error;
}



static ChannelReadImpl channelRead;

static void
channelRead(TChannel *      const channelP, 
            unsigned char * const buffer, 
            uint32_t        const bufferSize,
            uint32_t *      const bytesReceivedP,
            bool *          const failedP) {

    struct socketUnix * const socketUnixP = channelP->implP;

    int rc;
    rc = recv(socketUnixP->fd, buffer, bufferSize, 0);

    if (rc < 0) {
        *failedP = true;
        if (ChannelTraceIsActive)
            fprintf(stderr, "Abyss channel: "
                    "Failed to receive data from socket.  "
                    "recv() failed with errno %d (%s)\n",
                    errno, strerror(errno));
    } else {
        *failedP = false;
        *bytesReceivedP = rc;

        if (ChannelTraceIsActive)
            fprintf(stderr, "Abyss channel: read %u bytes: '%.*s'\n",
                    *bytesReceivedP, (int)(*bytesReceivedP), buffer);
    }
}



static ChannelWaitImpl channelWait;

static void
channelWait(TChannel * const channelP,
            bool       const waitForRead,
            bool       const waitForWrite,
            uint32_t   const timeoutMs,
            bool *     const readyToReadP,
            bool *     const readyToWriteP,
            bool *     const failedP) {
/*----------------------------------------------------------------------------
   Wait for the channel to be immediately readable or writable.

   Readable means there is at least one byte of data to read or the
   partner has disconnected.  Writable means the channel will take at
   least one byte of data to send or the partner has disconnected.

   'waitForRead' and 'waitForWrite' determine which of these
   conditions for which to wait; if both are true, we wait for either
   one.

   We return before the requested condition holds if 'timeoutMs'
   milliseconds pass.  timeoutMs == TIME_INFINITE means infinity.

   We return before the requested condition holds if the process receives
   (and catches) a signal, but only if it receives that signal a certain
   time after we start running.  (That means this function isn't useful
   for most purposes).

   Return *readyToReadP == true if the reason for returning is that
   the channel is immediately readable.  *readyToWriteP is analogous
   for writable.  Both may be true.

   Return *failedP true iff we fail to wait for the requested
   condition because of some unusual problem.  Being interrupted by a
   signal is not a failure.

   If one of these return value pointers is NULL, don't return that
   value.
-----------------------------------------------------------------------------*/
    struct socketUnix * const socketUnixP = channelP->implP;

    /* Design note: some old systems may not have poll().  We're assuming
       that we don't have to run on any such system.  select() is more
       universal, but can't handle a file descriptor with a high number.

       pselect() and ppoll() would allow us to be properly
       interruptible by a signal -- we would add a signal mask to our
       arguments.  But ppoll() is fairly rare.  pselect() is more
       common, but in older Linux systems it doesn't actually work.
    */
    bool readyToRead, readyToWrite, failed;
    struct pollfd pollfds[2];
    int rc;

    pollfds[0].fd = socketUnixP->fd;
    pollfds[0].events =
        (waitForRead  ? POLLIN  : 0) |
        (waitForWrite ? POLLOUT : 0);

    pollfds[1].fd = socketUnixP->interruptPipe.interrupteeFd;
    pollfds[1].events = POLLIN;
    
    rc = poll(pollfds, ARRAY_SIZE(pollfds),
              timeoutMs == TIME_INFINITE ? -1 : (int)timeoutMs);

    if (rc < 0) {
        if (errno == EINTR) {
            failed       = false;
            readyToRead  = false;
            readyToWrite = false;
        } else {
            failed       = true;
            readyToRead  = false; /* quiet compiler warning */
            readyToWrite = false; /* quiet compiler warning */
        }
    } else {
        failed       = false;
        readyToRead  = !!(pollfds[0].revents & POLLIN);
        readyToWrite = !!(pollfds[0].revents & POLLOUT);
    }

    if (failedP)
        *failedP       = failed;
    if (readyToReadP)
        *readyToReadP  = readyToRead;
    if (readyToWriteP)
        *readyToWriteP = readyToWrite;
}



static ChannelInterruptImpl channelInterrupt;

static void
channelInterrupt(TChannel * const channelP) {
/*----------------------------------------------------------------------------
  Interrupt any waiting that a thread might be doing in channelWait()
  now or in the future.

  TODO: Make a way to reset this so that future channelWait()s can once
  again wait.
-----------------------------------------------------------------------------*/
    struct socketUnix * const socketUnixP = channelP->implP;
    unsigned char const zero[1] = {0u};

    write(socketUnixP->interruptPipe.interruptorFd, &zero, sizeof(zero));
}



void
ChannelUnixGetPeerName(TChannel *         const channelP,
                       struct sockaddr ** const sockaddrPP,
                       size_t  *          const sockaddrLenP,
                       const char **      const errorP) {

    struct socketUnix * const socketUnixP = channelP->implP;

    unsigned char * peerName;
    socklen_t nameSize;

    nameSize = sizeof(struct sockaddr) + 1;
    
    peerName = malloc(nameSize);

    if (peerName == NULL)
        xmlrpc_asprintf(errorP, "Unable to allocate space for peer name");
    else {
        int rc;
        socklen_t nameLen;
        nameLen = nameSize;  /* initial value */
        rc = getpeername(socketUnixP->fd, (struct sockaddr *)peerName,
                         &nameLen);

        if (rc < 0)
            xmlrpc_asprintf(errorP, "getpeername() failed.  errno=%d (%s)",
                            errno, strerror(errno));
        else {
            if (nameLen > nameSize-1)
                xmlrpc_asprintf(errorP,
                                "getpeername() says the socket name is "
                                "larger than %u bytes, which means it is "
                                "not in the expected format.",
                                nameSize-1);
            else {
                *sockaddrPP = (struct sockaddr *)peerName;
                *sockaddrLenP = nameLen;
                *errorP = NULL;
            }
        }
        if (*errorP)
            free(peerName);
    }
}



static void
formatPeerInfoInet(const struct sockaddr_in * const sockaddrInP,
                   socklen_t                  const sockaddrLen,
                   const char **              const peerStringP) {

    if (sockaddrLen < sizeof(*sockaddrInP))
        xmlrpc_asprintf(peerStringP, "??? getpeername() returned "
                        "the wrong size");
    else {
        unsigned char * const ipaddr = (unsigned char *)
            &sockaddrInP->sin_addr.s_addr;
        xmlrpc_asprintf(peerStringP, "%u.%u.%u.%u:%hu",
                        ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
                        ntohs(sockaddrInP->sin_port));
    }
}



static void
formatPeerInfoInet6(const struct sockaddr_in6 * const sockaddrIn6P,
                    socklen_t                   const sockaddrLen,
                    const char **               const peerStringP) {

    if (sockaddrLen < sizeof(*sockaddrIn6P))
        xmlrpc_asprintf(peerStringP, "??? getpeername() returned "
                        "the wrong size");
    else {
        /* Gcc on Debian 6 gives a bewildering error message about aliasing
           if we try to dereference sockaddrIn6P in various ways, regardless
           of casts to void * or anything else.  So we copy the data structure
           as raw memory contents and then access the copy.  12.02.05.
        */
        struct sockaddr_in6 sockaddr;

        MEMSCPY(&sockaddr, sockaddrIn6P);

        {
            char buffer[256];
            const char * rc;

            rc = inet_ntop(AF_INET6, &sockaddr.sin6_addr,
                           buffer, sizeof(buffer));

            /* Punt the supposedly impossible error */
            if (rc == NULL)
                STRSCPY(buffer, "???");

            xmlrpc_asprintf(peerStringP, "[%s]:%hu",
                            buffer, sockaddr.sin6_port);
        }
    }
}



static ChannelFormatPeerInfoImpl channelFormatPeerInfo;

static void
channelFormatPeerInfo(TChannel *    const channelP,
                      const char ** const peerStringP) {

    struct socketUnix * const socketUnixP = channelP->implP;

    struct sockaddr sockaddr;
    socklen_t sockaddrLen;
    int rc;

    sockaddrLen = sizeof(sockaddr);
    
    rc = getpeername(socketUnixP->fd, &sockaddr, &sockaddrLen);
    
    if (rc < 0)
        xmlrpc_asprintf(peerStringP, "?? getpeername() failed.  errno=%d (%s)",
                        errno, strerror(errno));
    else {
        switch (sockaddr.sa_family) {
        case AF_INET:
            formatPeerInfoInet((const struct sockaddr_in *) &sockaddr,
                               sockaddrLen,
                               peerStringP);
            break;
        case AF_INET6:
            formatPeerInfoInet6((const struct sockaddr_in6 *) &sockaddr,
                                sockaddrLen,
                                peerStringP);
            break;
        default:
            xmlrpc_asprintf(peerStringP, "??? AF=%u", sockaddr.sa_family);
        }
    }
}



static struct TChannelVtbl const channelVtbl = {
    &channelDestroy,
    &channelWrite,
    &channelRead,
    &channelWait,
    &channelInterrupt,
    &channelFormatPeerInfo,
};



static void
makeChannelInfo(struct abyss_unix_chaninfo ** const channelInfoPP,
                struct sockaddr               const peerAddr,
                socklen_t                     const peerAddrLen,
                const char **                 const errorP) {

    struct abyss_unix_chaninfo * channelInfoP;

    MALLOCVAR(channelInfoP);
    
    if (channelInfoP == NULL)
        xmlrpc_asprintf(errorP, "Unable to allocate memory");
    else {
        channelInfoP->peerAddrLen = peerAddrLen;
        channelInfoP->peerAddr    = peerAddr;
        
        *errorP = NULL;
    }
    *channelInfoPP = channelInfoP;
}



static void
makeChannelFromFd(int           const fd,
                  TChannel **   const channelPP,
                  const char ** const errorP) {

    struct socketUnix * socketUnixP;

    MALLOCVAR(socketUnixP);
    
    if (socketUnixP == NULL)
        xmlrpc_asprintf(errorP, "Unable to allocate memory for Unix "
                        "channel descriptor");
    else {
        TChannel * channelP;
        
        socketUnixP->fd = fd;
        socketUnixP->userSuppliedFd = true;

        initInterruptPipe(&socketUnixP->interruptPipe, errorP);

        if (!*errorP) {
            ChannelCreate(&channelVtbl, socketUnixP, &channelP);
        
            if (channelP == NULL)
                xmlrpc_asprintf(errorP, "Unable to allocate memory for "
                                "channel descriptor.");
            else {
                *channelPP = channelP;
                *errorP = NULL;
            }
            if (*errorP)
                termInterruptPipe(socketUnixP->interruptPipe);
        }
        if (*errorP)
            free(socketUnixP);
    }
}



void
ChannelUnixCreateFd(int                           const fd,
                    TChannel **                   const channelPP,
                    struct abyss_unix_chaninfo ** const channelInfoPP,
                    const char **                 const errorP) {

    struct sockaddr peerAddr;
    socklen_t peerAddrLen;
    int rc;

    peerAddrLen = sizeof(peerAddr);

    rc = getpeername(fd, &peerAddr, &peerAddrLen);

    if (rc != 0) {
        if (errno == ENOTCONN)
            xmlrpc_asprintf(errorP, "Socket on file descriptor %d is not in "
                            "connected state.", fd);
        else
            xmlrpc_asprintf(errorP, "getpeername() failed on fd %d.  "
                            "errno=%d (%s)", fd, errno, strerror(errno));
    } else {
        makeChannelInfo(channelInfoPP, peerAddr, peerAddrLen, errorP);
        if (!*errorP) {
            makeChannelFromFd(fd, channelPP, errorP);

            if (*errorP)
                free(*channelInfoPP);
        }
    }
}



/*=============================================================================
      TChanSwitch
=============================================================================*/

static SwitchDestroyImpl chanSwitchDestroy;

static void
chanSwitchDestroy(TChanSwitch * const chanSwitchP) {

    struct socketUnix * const socketUnixP = chanSwitchP->implP;

    termInterruptPipe(socketUnixP->interruptPipe);

    if (!socketUnixP->userSuppliedFd)
        close(socketUnixP->fd);

    free(socketUnixP);
}



static SwitchListenImpl  chanSwitchListen;

static void
chanSwitchListen(TChanSwitch * const chanSwitchP,
                 uint32_t      const backlog,
                 const char ** const errorP) {

    struct socketUnix * const socketUnixP = chanSwitchP->implP;

    int32_t const minus1 = -1;

    int rc;

    /* Disable the Nagle algorithm to make persistant connections faster */

    setsockopt(socketUnixP->fd, IPPROTO_TCP, TCP_NODELAY,
               &minus1, sizeof(minus1));

    rc = listen(socketUnixP->fd, backlog);

    if (rc == -1)
        xmlrpc_asprintf(errorP, "listen() failed with errno %d (%s)",
                        errno, strerror(errno));
    else
        *errorP = NULL;
}



static void
waitForConnection(struct socketUnix * const listenSocketP,
                  bool *              const interruptedP,
                  const char **       const errorP) {
/*----------------------------------------------------------------------------
   Wait for the listening socket to have a connection ready to accept.

   We return before the requested condition holds if the process receives
   (and catches) a signal, but only if it receives that signal a certain
   time after we start running.  (That means this behavior isn't useful
   for most purposes).

   We furthermore return before the requested condition holds if someone sends
   a byte through the listening socket's interrupt pipe (or has sent one
   previously since the most recent time the pipe was drained).

   Return *interruptedP == true if we return before there is a connection
   ready to accept.
-----------------------------------------------------------------------------*/
    struct pollfd pollfds[2];
    int rc;

    pollfds[0].fd = listenSocketP->fd;
    pollfds[0].events = POLLIN;

    pollfds[1].fd = listenSocketP->interruptPipe.interrupteeFd;
    pollfds[1].events = POLLIN;
    
    rc = poll(pollfds, ARRAY_SIZE(pollfds), -1);

    if (rc < 0) {
        if (errno == EINTR) {
            *errorP       = NULL;
            *interruptedP = true;
        } else {
            xmlrpc_asprintf(errorP, "poll() failed, errno = %d (%s)",
                            errno, strerror(errno));
            *interruptedP = false; /* quiet compiler warning */
        }
    } else if (pollfds[0].revents & POLLHUP) {
        xmlrpc_asprintf(errorP, "INTERNAL ERROR: listening socket "
                        "is not listening");
    } else if (pollfds[1].revents & POLLHUP) {
        xmlrpc_asprintf(errorP, "INTERNAL ERROR: interrupt socket hung up");
    } else if (pollfds[0].revents & POLLERR) {
        xmlrpc_asprintf(errorP, "listening socket is in POLLERR status");
    } else if (pollfds[1].revents & POLLHUP) {
        xmlrpc_asprintf(errorP, "interrupt socket is in POLLERR status");
    } else {
        *errorP       = NULL;
        *interruptedP = !(pollfds[0].revents & POLLIN);
    }
}



static void
createChannelForAccept(int             const acceptedFd,
                       struct sockaddr const peerAddr,
                       TChannel **     const channelPP,
                       void **         const channelInfoPP,
                       const char **   const errorP) {
/*----------------------------------------------------------------------------
   Make a channel object (TChannel) out of a socket just created by
   accept() on a listening socket -- i.e. a socket for a client connection.

   'acceptedFd' is the file descriptor of the socket.

   'peerAddr' is the address of the client, from accept().
-----------------------------------------------------------------------------*/
    struct abyss_unix_chaninfo * channelInfoP;

    makeChannelInfo(&channelInfoP, peerAddr, sizeof(peerAddr), errorP);
    if (!*errorP) {
        struct socketUnix * acceptedSocketP;

        MALLOCVAR(acceptedSocketP);

        if (!acceptedSocketP)
            xmlrpc_asprintf(errorP, "Unable to allocate memory");
        else {
            acceptedSocketP->fd = acceptedFd;
            acceptedSocketP->userSuppliedFd = false;

            initInterruptPipe(&acceptedSocketP->interruptPipe, errorP);

            if (!*errorP) {
                TChannel * channelP;

                ChannelCreate(&channelVtbl, acceptedSocketP, &channelP);
                if (!channelP)
                    xmlrpc_asprintf(errorP,
                                    "Failed to create TChannel object.");
                else {
                    *errorP        = NULL;
                    *channelPP     = channelP;
                    *channelInfoPP = channelInfoP;
                }
                if (*errorP)
                    termInterruptPipe(acceptedSocketP->interruptPipe);
            }
            if (*errorP)
                free(acceptedSocketP);
        }
        if (*errorP)
            free(channelInfoP);
    }
}



static SwitchAcceptImpl chanSwitchAccept;

static void
chanSwitchAccept(TChanSwitch * const chanSwitchP,
                 TChannel **   const channelPP,
                 void **       const channelInfoPP,
                 const char ** const errorP) {
/*----------------------------------------------------------------------------
   Accept a connection via the channel switch *chanSwitchP.  Return as
   *channelPP the channel for the accepted connection.

   If no connection is waiting at *chanSwitchP, wait until one is.

   If we receive a signal while waiting, return immediately with
   *channelPP == NULL.
-----------------------------------------------------------------------------*/
    struct socketUnix * const listenSocketP = chanSwitchP->implP;

    bool interrupted;
    TChannel * channelP;

    interrupted = false; /* Haven't been interrupted yet */
    channelP    = NULL;  /* No connection yet */
    *errorP     = NULL;  /* No error yet */

    while (!channelP && !*errorP && !interrupted) {

        waitForConnection(listenSocketP, &interrupted, errorP);

        if (!*errorP && !interrupted) {
            struct sockaddr peerAddr;
            socklen_t size = sizeof(peerAddr);
            int rc;

            rc = accept(listenSocketP->fd, &peerAddr, &size);

            if (rc >= 0) {
                int const acceptedFd = rc;

                createChannelForAccept(acceptedFd, peerAddr,
                                       &channelP, channelInfoPP, errorP);

                if (*errorP)
                    close(acceptedFd);
            } else if (errno == EINTR)
                interrupted = true;
            else
                xmlrpc_asprintf(errorP, "accept() failed, errno = %d (%s)",
                                errno, strerror(errno));
        }
    }
    *channelPP = channelP;
}



static SwitchInterruptImpl chanSwitchInterrupt;

static void
chanSwitchInterrupt(TChanSwitch * const chanSwitchP) {
/*----------------------------------------------------------------------------
  Interrupt any waiting that a thread might be doing in chanSwitchAccept()
  now or in the future.

  TODO: Make a way to reset this so that future chanSwitchAccept()s can once
  again wait.
-----------------------------------------------------------------------------*/
    struct socketUnix * const listenSocketP = chanSwitchP->implP;

    unsigned char const zero[1] = {0u};

    write(listenSocketP->interruptPipe.interruptorFd, &zero, sizeof(zero));
}



static struct TChanSwitchVtbl const chanSwitchVtbl = {
    &chanSwitchDestroy,
    &chanSwitchListen,
    &chanSwitchAccept,
    &chanSwitchInterrupt,
};



static void
createChanSwitch(int            const fd,
                 bool           const userSuppliedFd,
                 TChanSwitch ** const chanSwitchPP,
                 const char **  const errorP) {

    struct socketUnix * socketUnixP;

    assert(!connected(fd));

    if (SwitchTraceIsActive)
        fprintf(stderr, "Creating Unix listen-socket based channel switch\n");

    MALLOCVAR(socketUnixP);

    if (socketUnixP == NULL)
        xmlrpc_asprintf(errorP, "unable to allocate memory for Unix "
                        "channel switch descriptor.");
    else {
        TChanSwitch * chanSwitchP;

        socketUnixP->fd = fd;
        socketUnixP->userSuppliedFd = userSuppliedFd;
            
        initInterruptPipe(&socketUnixP->interruptPipe, errorP);

        if (!*errorP) {
            ChanSwitchCreate(&chanSwitchVtbl, socketUnixP, &chanSwitchP);
            if (*errorP)
                termInterruptPipe(socketUnixP->interruptPipe);

            if (chanSwitchP == NULL)
                xmlrpc_asprintf(errorP, "Unable to allocate memory for "
                                "channel switch descriptor");
            else {
                *chanSwitchPP = chanSwitchP;
                *errorP = NULL;
            }
        }
        if (*errorP)
            free(socketUnixP);
    }
}



static void
setSocketOptions(int           const fd,
                 const char ** const errorP) {

    int32_t n = 1;
    int rc;

    rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n));

    if (rc < 0)
        xmlrpc_asprintf(errorP, "Failed to set socket options.  "
                        "setsockopt() failed with errno %d (%s)",
                        errno, strerror(errno));
    else
        *errorP = NULL;
}



static void
traceSocketBound(const struct sockaddr * const sockAddrP,
                 socklen_t               const sockAddrLen) {

    if (sockAddrP->sa_family == AF_INET &&
        sockAddrLen >= sizeof(struct sockaddr_in)) {
        const struct sockaddr_in * const sockAddrInP =
            (const struct sockaddr_in *)sockAddrP;

        unsigned char * const ipaddr = (unsigned char *)
            &sockAddrInP->sin_addr.s_addr;

        fprintf(stderr, "Bound socket for channel switch to "
                "AF_INET port %u.%u.%u.%u:%hu\n",
                ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
                ntohs(sockAddrInP->sin_port));
    } else {
        fprintf(stderr, "Bound socket for channel switch to address of "
                "family %d\n", sockAddrP->sa_family);
    }
}



static void
bindSocketToPort(int                     const fd,
                 const struct sockaddr * const sockAddrP,
                 socklen_t               const sockAddrLen,
                 const char **           const errorP) {
    
    int rc;

    rc = bind(fd, sockAddrP, sockAddrLen);

    if (rc == -1)
        xmlrpc_asprintf(errorP, "Unable to bind socket "
                        "to the socket address.  "
                        "bind() failed with errno %d (%s)",
                        errno, strerror(errno));
    else {
        *errorP = NULL;

        if (SwitchTraceIsActive)
            traceSocketBound(sockAddrP, sockAddrLen);
    }
}



static void
bindSocketToPortInet(int           const fd,
                     uint16_t      const portNumber,
                     const char ** const errorP) {
    
    struct sockaddr_in name;
    int rc;

    name.sin_family = AF_INET;
    name.sin_port   = htons(portNumber);
    name.sin_addr.s_addr = INADDR_ANY;

    rc = bind(fd, (struct sockaddr *)&name, sizeof(name));

    if (rc == -1)
        xmlrpc_asprintf(errorP, "Unable to bind IPv4 socket "
                        "to port number %hu.  "
                        "bind() failed with errno %d (%s)",
                        portNumber, errno, strerror(errno));
    else
        *errorP = NULL;
}



static void
bindSocketToPortInet6(int                     const fd,
                      uint16_t                const portNumber,
                      const char **           const errorP) {

    struct sockaddr_in6 name;
    int rc;

    name.sin6_family = AF_INET6;
    name.sin6_port   = htons(portNumber);
    name.sin6_addr = in6addr_any;

    rc = bind(fd, (struct sockaddr *)&name, sizeof(name));

    if (rc == -1)
        xmlrpc_asprintf(errorP, "Unable to bind IPv6 socket "
                        "to port number %hu.  "
                        "bind() failed with errno %d (%s)",
                        portNumber, errno, strerror(errno));
    else
        *errorP = NULL;
}



static void
switchCreateIpV4Port(unsigned short          const portNumber,
                     TChanSwitch **          const chanSwitchPP,
                     const char **           const errorP) {
/*----------------------------------------------------------------------------
   Create a POSIX-socket-based channel switch for an IPv4 endpoint.

   Set the socket's local address so that a subsequent "listen" will listen on
   all interfaces, port number 'portNumber'.
-----------------------------------------------------------------------------*/
    int rc;
    rc = socket(PF_INET, SOCK_STREAM, 0);
    if (rc < 0)
        xmlrpc_asprintf(errorP, "socket() failed with errno %d (%s)",
                        errno, strerror(errno));
    else {
        int const socketFd = rc;

        setSocketOptions(socketFd, errorP);
        if (!*errorP) {
            bindSocketToPortInet(socketFd, portNumber, errorP);

            if (!*errorP) {
                bool const userSupplied = false;
                createChanSwitch(socketFd, userSupplied, chanSwitchPP, errorP);
            }
        }
        if (*errorP)
            close(socketFd);
    }
}



static void
switchCreateIpV6Port(unsigned short          const portNumber,
                     TChanSwitch **          const chanSwitchPP,
                     const char **           const errorP) {
/*----------------------------------------------------------------------------
  Same as switchCreateIpV4Port(), except for IPv6.
-----------------------------------------------------------------------------*/
    int rc;
    rc = socket(PF_INET6, SOCK_STREAM, 0);
    if (rc < 0)
        xmlrpc_asprintf(errorP, "socket() failed with errno %d (%s)",
                        errno, strerror(errno));
    else {
        int const socketFd = rc;

        setSocketOptions(socketFd, errorP);
        if (!*errorP) {
            bindSocketToPortInet6(socketFd, portNumber, errorP);

            if (!*errorP) {
                bool const userSupplied = false;
                createChanSwitch(socketFd, userSupplied, chanSwitchPP, errorP);
            }
        }
        if (*errorP)
            close(socketFd);
    }
}



void
ChanSwitchUnixCreate(unsigned short const portNumber,
                     TChanSwitch ** const chanSwitchPP,
                     const char **  const errorP) {

    switchCreateIpV4Port(portNumber, chanSwitchPP, errorP);
}



void
ChanSwitchUnixCreate2(int                     const protocolFamily,
                      const struct sockaddr * const sockAddrP,
                      socklen_t               const sockAddrLen,
                      TChanSwitch **          const chanSwitchPP,
                      const char **           const errorP) {

    int rc;
    rc = socket(protocolFamily, SOCK_STREAM, 0);
    if (rc < 0)
        xmlrpc_asprintf(errorP, "socket() failed with errno %d (%s)",
                        errno, strerror(errno));
    else {
        int const socketFd = rc;

        if (SwitchTraceIsActive)
            fprintf(stderr, "Created socket for protocol family %d\n",
                    protocolFamily);

        setSocketOptions(socketFd, errorP);
        if (!*errorP) {
            bindSocketToPort(socketFd, sockAddrP, sockAddrLen, errorP);

            if (!*errorP) {
                bool const userSupplied = false;
                createChanSwitch(socketFd, userSupplied, chanSwitchPP, errorP);
            }
        }
        if (*errorP)
            close(socketFd);
    }

}



void
ChanSwitchUnixCreateIpV6Port(unsigned short const portNumber,
                             TChanSwitch ** const chanSwitchPP,
                             const char **  const errorP) {

    switchCreateIpV6Port(portNumber, chanSwitchPP, errorP);
}



void
ChanSwitchUnixCreateFd(int            const fd,
                       TChanSwitch ** const chanSwitchPP,
                       const char **  const errorP) {

    if (connected(fd))
        xmlrpc_asprintf(errorP,
                        "Socket (file descriptor %d) is in connected "
                        "state.", fd);
    else {
        bool const userSupplied = true;
        createChanSwitch(fd, userSupplied, chanSwitchPP, errorP);
    }
}



/*=============================================================================
      obsolete TSocket interface
=============================================================================*/


void
SocketUnixCreateFd(int        const fd,
                   TSocket ** const socketPP) {

    TSocket * socketP;
    const char * error;

    if (connected(fd)) {
        TChannel * channelP;
        struct abyss_unix_chaninfo * channelInfoP;
        ChannelUnixCreateFd(fd, &channelP, &channelInfoP, &error);
        if (!error)
            SocketCreateChannel(channelP, channelInfoP, &socketP);
    } else {
        TChanSwitch * chanSwitchP;
        ChanSwitchUnixCreateFd(fd, &chanSwitchP, &error);
        if (!error)
            SocketCreateChanSwitch(chanSwitchP, &socketP);
    }
    if (error) {
        *socketPP = NULL;
        xmlrpc_strfree(error);
    } else
        *socketPP = socketP;
}



