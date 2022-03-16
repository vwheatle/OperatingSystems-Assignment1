#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)

#include <errno.h> // -> errno

// #include <sys/types.h> // -> types for shared memory
// #include <sys/mman.h>  // -> shared memory
// #include <sys/stat.h>  // -> mode constants; fstat
// #include <fcntl.h>     // -> O_* constants

#include <semaphore.h> // -> semaphores

#include "shared_memory.h" // -> table_t; TABLE_NAME; initializeTable

#define PROC_LABEL_STR "\033[96m"  "Consumer"  "\033[0m: "
#include "process_sync.h"

void meat(table_t* table) {
	table_item_t myItem;
	int iterationsLeft = TABLE_ITERATIONS;
	
	printf_label("Starting.\n");
	
	do {
		sem_wait(&table->full ); // Get 1 item.
		sem_wait(&table->mutex); // Enter critical section.
		
		// Take from the buffer.
		myItem = getItem(table);
		
		sem_post(&table->mutex); // Exit critical section.
		sem_post(&table->empty); // Put 1 empty space.
		
		// Consume something.
		printf_label("Ate %d.\n", myItem);
		
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
