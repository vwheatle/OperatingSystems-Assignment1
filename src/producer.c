#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)

#include <semaphore.h> // -> semaphores

#include "shared_memory.h" // -> table_t; TABLE_NAME; initializeTable

// This will be displayed alongside the process' printf_label statements,
// as well as the process' perrors if any. (I hope not any!)
#define PROC_LABEL_STR "\033[95m"  "Producer"  "\033[0m: "
#include "process_sync.h" // -> {open|close}TableSync, printf_label, ...

void meat(table_t* table) {
	table_item_t myItem = 0;
	int iterationsLeft = TABLE_ITERATIONS;
	
	printf_label("Starting.\n");
	
	do {
		// Produce something.
		myItem = rand() & 0xFF;
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
	
	// Get the table_t struct.
	table_t* table = openTableSync();
	
	// openTableSync (after printing error information)
	// returns a null pointer if the open failed.
	if (table == NULL) {
		exit(EXIT_FAILURE);
	}
	
	// Otherwise, run the real program.
	// The "Meat and Potatoes".
	meat(table);
	
	// Afterwards, close the table -- and if it's not zero, then something
	// went wrong. Even if that happens, we still exit the program.
	bool successful = closeTableSync(table) == 0;
	exit(successful ? EXIT_SUCCESS : EXIT_FAILURE);
}
