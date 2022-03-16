#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)

#include <errno.h> // -> errno

#include <sys/types.h> // -> types for shared memory
#include <sys/mman.h>  // -> shared memory
#include <sys/stat.h>  // -> mode constants; fstat
#include <fcntl.h>     // -> O_* constants

#include <semaphore.h> // -> semaphores

#include "shared_memory.h" // -> table_t; TABLE_NAME; initializeTable

#define PROC_LABEL_STR "\033[95m"  "Producer"  "\033[0m: "
#include "process_sync.h"

void meat(table_t* table) {
	table_item_t myItem = 0;
	int iterationsLeft = TABLE_ITERATIONS;
	
	printf_label("Starting.\n");
	
	do {
		// Produce something.
		// myItem = rand() & 0xFF;
		myItem++;
		printf_label("Made %d.\n", myItem);
		
		sem_wait(&table->empty); // Get 1 empty space.
		sem_wait(&table->mutex); // Enter critical section.
		
		// Put it in the buffer.
		putItem(table, myItem);
		
		sem_post(&table->mutex); // Exit critical section.
		sem_post(&table->full ); // Put 1 item.
		
		iterationsLeft--;
	} while (iterationsLeft > 0);
	
	printf_label("Stopping.\n");
}

int main() {
	printf_label("Hello, world!\n");
	
	table_t* table = openTableSync();
	if (table == NULL) exit(EXIT_FAILURE);
	
	meat(table);
	
	if (closeTableSync(table) != 0) exit(EXIT_FAILURE);
	
	exit(EXIT_SUCCESS);
}
