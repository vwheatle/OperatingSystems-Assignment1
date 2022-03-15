#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)
#include <unistd.h>  // -> sleep, getpid

#include <errno.h> // -> errno

#include <sys/types.h> // -> types for shared memory
#include <sys/mman.h>  // -> shared memory
#include <sys/stat.h>  // -> mode constants; fstat
#include <fcntl.h>     // -> O_* constants

#include <semaphore.h> // -> semaphores

#include "shared_memory.h" // -> table_t; TABLE_NAME; initializeTable

#define PROC_LABEL_STR "\033[95mProducer\033[0m: "

#define printf_label(...) printf(PROC_LABEL_STR); printf(__VA_ARGS__)
#define perror_label(message) printf(PROC_LABEL_STR); perror(message)

#define perror_exit(message) do { perror_label(message); exit(EXIT_FAILURE); } while (0)
// do {} while (0) used in man pages, explained https://stackoverflow.com/a/9496007/

void meat(table_t* table) {
	table_item_t myItem;
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
	bool successful = false;
	
	printf_label("Hello, world!\n");
	
	// Should this process be the one to initialize the shared memory?
	bool shouldInitialize = false;
	
	int fd = shm_open(TABLE_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd >= 0) {
		// This process was the first to open the shared memory, and
		// all future processes will wait for the shm_struct->isReady signal.
		shouldInitialize = true;
	} else if (errno == EEXIST) {
		// If the first open failed and the error was "already exists",
		// someone else is doing initialization.
		shouldInitialize = false; // Redundant for clarity.
		
		printf_label("Not in charge of shared memory initialization.\n");
		
		// Since the first shm_open failed, we'll have to try again.
		// This one has less protective flags, so it's more likely to
		// succeed -- however, we should still handle if it fails.
		fd = shm_open(TABLE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	}
	if (fd < 0) {
		// This catches both a non-EEXIST first shm_open,
		//              and every failed second shm_open.
		perror_exit("Failed to open shared memory");
		// "Never write clever code. Write clear code." *writes clever code*
	}
	
	// Shared memory starts at 0 bytes.
	// Resize it to the actual amount needed.
	// This won't do anything bad if all processes do it.
	if (ftruncate(fd, sizeof(table_t)) < 0) {
		perror_label("Extending shared memory failed");
		goto die;
	}
	
	// Map the shared memory into this process' address space.
	// This returns a pointer to what is essentially our struct.
	table_t* table = mmap(
		NULL, sizeof(table_t),  // address & size
		PROT_READ | PROT_WRITE, // page permissions
		MAP_SHARED, fd, 0       // other stuff
	);
	if (table == MAP_FAILED) {
		perror_label("Memory mapping failed");
		goto die;
	}
	
	// We've already decided if this process should initialize the table
	// in the lines above, but yeah -- only one process shall initialize
	// the table. Otherwise the semaphore would be initialized twice!!!
	if (shouldInitialize) {
		initializeTable(table);
		printf_label("Done initializing.\n");
	} else {
		printf_label("Waiting for ready flag...\n");
		while (!table->isReady) usleep(100);
	}
	
	// Table is completely working after this part.
	
	// Arrive
	sem_wait(&table->mutex);
	table->population++;
	sem_post(&table->mutex);
	
	meat(table);
	
	// Leave
	sem_wait(&table->mutex);
	table->population--;
	bool hasCleanupDuty = table->population == 0;
	sem_post(&table->mutex);
	
	successful = true;
	
	if (hasCleanupDuty) {
		printf_label("Cleaning up.\n");
		destroyTable(table);
	}
	
	// Unmap shared memory from process' address space.
	munmap(table, sizeof(table_t));
	table = NULL;
	
die:
	// Closes shared memory handle...
	close(fd);
	// ...and unlinks shared memory name.
	shm_unlink(TABLE_NAME);
	
	exit(successful ? EXIT_SUCCESS : EXIT_FAILURE);
}
