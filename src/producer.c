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
	do {
		// Produce something.
		
		sem_wait(&table->empty);
		sem_wait(&table->mutex);
		
		// Put it in the buffer.
		
		sem_post(&table->mutex);
		sem_post(&table->full);
	} while (true);
}

int main() {
	// TODO: see if test_sharedmem.c's way of error quitting was the right idea.
	// I'm wondering if it's fine to let the operating system clean up most of
	// the resources for me. It might also be a jerk move.
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
		perror_exit("Extending shared memory failed");
	}
	
	// Map the shared memory into this program's addr. space.
	// This returns a pointer to what is essentially our struct.
	table_t* table = mmap(
		NULL, sizeof(table_t),  // address & size
		PROT_READ | PROT_WRITE, // page permissions
		MAP_SHARED, fd, 0       // other stuff
	);
	if (table == MAP_FAILED) {
		perror_exit("Memory mapping failed");
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
	
	meat(table);
	
	successful = true;
	
die_unmap:
	// Inverse of mmap
	munmap(table, sizeof(table_t));
die:
	// Closes shared memory handle...
	close(fd);
	// ...and unlinks shared memory name.
	shm_unlink(TABLE_NAME);
	
	exit(successful ? EXIT_SUCCESS : EXIT_FAILURE);
}
