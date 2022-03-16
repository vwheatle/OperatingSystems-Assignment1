#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)

#include <semaphore.h> // -> semaphores

#include "shared_memory.h" // -> table_t; TABLE_NAME; initializeTable

// This will be displayed alongside the process' printf_label statements,
// as well as the process' perrors if any. (I hope not any!)
#define PROC_LABEL_STR "\033[96m"  "Consumer"  "\033[0m: "
#include "process_sync.h" // -> {open|close}TableSync, printf_label, ...

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
