/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 30-2 */

/* thread_incr_mutex.c

   This program employs two POSIX threads that increment the same global
   variable, synchronizing their access using a mutex. As a consequence,
   updates are not lost. Compare with thread_incr.c, thread_incr_spinlock.c,
   and thread_incr_rwlock.c.
*/
#include <pthread.h>
#include <signal.h>
#include "tlpi_hdr.h"

static volatile int glob = 0;
//static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *                   /* Loop 'arg' times incrementing 'glob' */
threadFunc(void *arg)
{
    while(1)
	{		 
		time_t current_time;
        time(&current_time);
		char *t = ctime(&current_time);
		if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
        pthread_t pid = pthread_self();
		printf("%s, I am : %lu\n", t, pid);
		sleep(2);
	}
    

    return NULL;
}

void sig_term_handler()
{
	// Kill all threads
    pthread_t pid = pthread_self();
	printf("in sig_term my id: %lu\n", pid);	
	exit(0);
}

void sig_pipe_handler()
{
	pthread_t pid = pthread_self();
	printf("in sig_pipe my id: %lu\n", pid);	
	exit(0);
}

int
main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;
    signal(SIGINT, sig_term_handler);
	signal(SIGPIPE, sig_pipe_handler);

    for (int i=0; i < 10; i++){
        s = pthread_create(&t1, NULL, threadFunc, NULL);
        if (s != 0)
            errExitEN(s, "pthread_create");
    }
    
    while(1){
        time_t current_time;
        time(&current_time);
		char *t = ctime(&current_time);
		if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
        pthread_t pid = pthread_self();
		printf("%s, I am main: %lu\n", t, pid);
		sleep(3);
    }    

    
    exit(EXIT_SUCCESS);
}
