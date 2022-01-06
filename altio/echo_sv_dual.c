/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Exercise 63-2 */

/* echo_sv_dual.c

   An implementation of dual protocol (TCP/UDP) "echo" service.

   NOTE: this program must be run under a root login, in order to allow the
   "echo" port (7) to be bound. Alternatively, for test purposes, you can
   replace the SERVICE name below with a suitable unreserved port number
   (e.g., "51000"), and make a corresponding change in the client.

   See also is_echo_cl.c.
*/
#include <signal.h>
#include <syslog.h>
#include <sys/wait.h>
#include "become_daemon.h"
#include "inet_sockets.h"       /* Declarations of inet*() socket functions */
#include "tlpi_hdr.h"

#define SERVICE "echo"          /* Name of TCP service */
#define BUF_SIZE 4096

static void             /* SIGCHLD handler to reap dead child processes */
grimReaper(int sig)
{
    int savedErrno;             /* Save 'errno' in case changed here */

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

/* Handle a client request: copy socket input back to socket */

static void
handleRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while ((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
        if (write(cfd, buf, numRead) != numRead) {
            syslog(LOG_ERR, "write() failed: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1) {
        syslog(LOG_ERR, "Error from read(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void
handleRequestDgram(int sfd){
    ssize_t numRead;
    socklen_t len;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE];
    char addrStr[IS_ADDR_STR_LEN];
    len = sizeof(struct sockaddr_storage);
    numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                        (struct sockaddr *) &claddr, &len);
    if (numRead == -1)
        errExit("recvfrom");

    if (sendto(sfd, buf, numRead, 0, (struct sockaddr *) &claddr, len)
                    != numRead)
        syslog(LOG_WARNING, "Error echoing response to %s (%s)",
                inetAddressStr((struct sockaddr *) &claddr, len,
                                addrStr, IS_ADDR_STR_LEN),
                strerror(errno));
}

int
main(int argc, char *argv[])
{
    int lfd, cfd, sfd;               /* Listening and connected sockets */
    struct sigaction sa;

    //if (becomeDaemon(0) == -1)
    //    errExit("becomeDaemon");

    /* Establish SIGCHLD handler to reap terminated child processes */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Error from sigaction(): %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inetListen(SERVICE, 10, NULL);
    if (lfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        perror("Could not create server socket");
        exit(EXIT_FAILURE);
    }

    sfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
    if (sfd == -1) {
        syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct timeval ts;
    ts.tv_sec = 1; // 1 second
    ts.tv_usec = 0;

    printf("starting server...\n");

    for (;;) {
        

        fd_set fds;
        FD_ZERO(&fds);
        if (sfd != 0)
            FD_SET(sfd, &fds);
        if (lfd != 0)
            FD_SET(lfd, &fds);        
        

        /* Handle each client request in a new child process */
        int nready = select(sfd + 1, &fds, (fd_set *) 0, (fd_set *) 0, &ts);
        if (nready < 0) {            
            syslog(LOG_ERR, "Failure in select(): %s", strerror(errno));
			/* The code which calls write (or other blocking operations) has to be aware of EINTR. If a signal occurs
			 * during a blocking operation, then the operation will either (a) return partial completion, or (b) return
			 * failure, do nothing, and set errno to EINTR.
			 * we want select to continue when signal processing for child proc happens*/
			if (errno == EINTR) continue;
            exit(EXIT_FAILURE);
        }           
        else if (nready == 0) {
            ts.tv_sec = 1; // 1 second
            ts.tv_usec = 0;
        }
        else if (lfd != 0 && FD_ISSET(lfd, &fds)) {
            cfd = accept(lfd, NULL, NULL);  /* Wait for connection */
            if (cfd == -1) {
                syslog(LOG_ERR, "Failure in accept(): %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            switch (fork()) {
                case -1:
                    syslog(LOG_ERR, "Can't create child (%s)", strerror(errno));
                    close(cfd);                 /* Give up on this client */
                    break;                      /* May be temporary; try next client */

                case 0:                         /* Child */
                    close(lfd);                 /* Unneeded copy of listening socket */
                    handleRequest(cfd);
                    _exit(EXIT_SUCCESS);

                default:                        /* Parent */
                    close(cfd);                 /* Unneeded copy of connected socket */
                    break;                      /* Loop to accept next connection */
            }
        }
        else if (sfd != 0 && FD_ISSET(sfd, &fds)) {
            handleRequestDgram(sfd);
        }

    }
}
