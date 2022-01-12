#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


extern volatile int glob;
extern pthread_mutex_t mutex;

void increment_clients_count();
//void monitor();

#endif
