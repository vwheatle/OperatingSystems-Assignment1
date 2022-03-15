#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)

#include <semaphore.h> // -> semaphores

#define TABLE_NAME "/myProdCnsmTable"
#define TABLE_SPACE 2

typedef struct {
	sem_t mutex;
	sem_t full, empty;
	bool isReady;
} table_t;

// Set the table. (Forks, knives, plates not included)
void initializeTable(table_t* table) {
	sem_init(&table->mutex, /*Procs share?*/ 1, /*Quantity*/ 1);
	
	sem_init(&table->full , /*Procs share?*/ 1, /*Quantity*/ 0);
	sem_init(&table->empty, /*Procs share?*/ 1, /*Quantity*/ TABLE_SPACE);
	
	// Shared memory, when initialized, is zero bytes long. Each process that
	// gets the shared memory object must use ftruncate to expand the memory
	// object to the size of table_t. ftruncate will fill any new space with
	// zeroes, so I take advantage of bools being set to false by default as
	// a result of this zeroing to provide a "ready" flag -- processes
	// waiting on shared memory initialization sleep while it's false,
	// so that the initialization routine completes before anyone touches
	// the data.
	table->isReady = true;
	// If you're wondering why this is a regular old boolean rather than
	// some sort of atomic or semaphore or mutex or something, it's because
	// I absolutely know that ONE and only one process will run this function.
	// This is thanks to the O_EXCL flag in shm_open, which will error the
	// open if the shared memory already exists. It's kinda a mutex lock on
	// the shared memory while a process initializes it.
	
	// I worried about mutexes more when I was experimenting with more than 2
	// processes running at a time -- I essentially made a system that decides
	// which process "got there first" and puts it in charge of setup.
	// In the producer/consumer project, I'm planning to recreate this *AND*
	// put *ONLY* the producer in charge of cleanup afterwards.
}
