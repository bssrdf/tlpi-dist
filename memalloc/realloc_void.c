/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 7-1 */

/* free_and_sbrk.c

   Test if free(3) actually lowers the program break.

   Usage: free_and_sbrk num-allocs block-size [step [min [max]]]

   Try: free_and_sbrk 1000 10240 2 1 1000
        free_and_sbrk 1000 10240 1 1 999
        free_and_sbrk 1000 10240 1 500 1000

        (Only the last of these should see the program break lowered.)
*/
#define _BSD_SOURCE
#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000


typedef struct {

    unsigned int slabs;     /* how many slabs were allocated for this class */

    void **slab_list;       /* array of slab pointers */
    unsigned int list_size; /* size of prev array */
} slabclass_t;

int
main(int argc, char *argv[])
{
    

    slabclass_t *p = (slabclass_t *) malloc(sizeof(slabclass_t));
	memset(p, 0, sizeof(slabclass_t)); 
	if(p->slab_list == NULL)
	   printf("pointer is set to NULL by memset\n");
	else
	   printf("pointer is not set to NULL by memset\n");
	   

    size_t new_size =  16;
    void *new_list = realloc(p->slab_list, new_size * sizeof(void *));
    p->list_size = new_size;
    p->slab_list = new_list;

	free(p->slab_list); 

	free(p);

    exit(EXIT_SUCCESS);
}
