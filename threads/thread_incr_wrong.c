#include "common.h"



void                    /* Loop 'arg' times incrementing 'glob' */
monitor()
{
    while(1)
	{		 
		time_t current_time;
        time(&current_time);
		char *t = ctime(&current_time);
		if (t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\0';
		printf("%s, Count : %d\n", t, glob);
		sleep(3);
	}

}



int
main(int argc, char *argv[])
{
    pthread_t t1;   
    int s;
   

    s = pthread_create(&t1, NULL, (void*)monitor, NULL);
    if (s != 0){
        perror("pthread_create");
        exit(1);
     }
    
    while(1){
        // pthread_mutex_lock(&mutex);		
		// glob++;
        // pthread_mutex_unlock(&mutex);
        increment_clients_count();        
        sleep(5);
    }

    
    exit(0);
}
