#include "common.h"

volatile int glob = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void increment_clients_count()
{
        // Lock
        pthread_mutex_lock(&mutex);		
        glob++;
        // Unlock
        printf("Count: %d\n", glob);
        pthread_mutex_unlock(&mutex);
}

// void                    /* Loop 'arg' times incrementing 'glob' */
// monitor()
// {
//     while(1)
// 	{		 
// 		time_t current_time;
//         time(&current_time);
// 		char *t = ctime(&current_time);
// 		if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
// 		printf("%s, Count : %d\n", t, glob);
// 		sleep(3);
// 	}

// }
