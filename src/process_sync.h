#pragma once

#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)
#include <unistd.h>  // -> usleep, getpid

#include <errno.h> // -> errno

#include <sys/types.h> // -> types for shared memory
#include <sys/mman.h>  // -> shared memory
#include <sys/stat.h>  // -> mode constants; fstat
#include <fcntl.h>     // -> O_* constants

#include "shared_memory.h"

#ifndef PROC_LABEL_STR
# define PROC_LABEL_STR "\033[97m" "Unknown" "\033[0m: "
#endif

#define printf_label(...) printf(PROC_LABEL_STR); printf(__VA_ARGS__)
#define perror_label(message) printf(PROC_LABEL_STR); perror(message)

// #define perror_exit(message) do { perror_label(message); exit(EXIT_FAILURE); } while (0)
// do {} while (0) used in man pages, explained https://stackoverflow.com/a/9496007/

table_t* openTableSync() {
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
		perror_label("Failed to open shared memory");
		return NULL;
		// "Never write clever code. Write clear code." *writes clever code*
	}
	
	// Shared memory starts at 0 bytes.
	// Resize it to the actual amount needed.
	// This won't do anything bad if all processes do it.
	if (ftruncate(fd, sizeof(table_t)) < 0) {
		perror_label("Extending shared memory failed");
		if (shouldInitialize) shm_unlink(TABLE_NAME);
		return NULL;
	}
	
	// Map the shared memory into this process' address space.
	// This returns a pointer to what is essentially our struct.
	table_t* table = (table_t*)mmap(
		NULL, sizeof(table_t),  // address & size
		PROT_READ | PROT_WRITE, // page permissions
		MAP_SHARED, fd, 0       // other stuff
	);
	
	// Don't need file descriptor anymore.
	close(fd);
	
	// Oh wait, mmap might've errored.
	if (table == MAP_FAILED) {
		perror_label("Memory mapping failed");
		return NULL;
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
		// TODO: this should have a "too long" condition that quits.
	}
	
	sem_wait(&table->mutex);
	table->population++;
	sem_post(&table->mutex);
	
	return table;
}

int closeTableSync(table_t* table) {
	sem_wait(&table->mutex);
	table->population--;
	bool hasCleanupDuty = table->population == 0;
	sem_post(&table->mutex);
	
	if (hasCleanupDuty) {
		printf_label("Cleaning up.\n");
		destroyTable(table);
		shm_unlink(TABLE_NAME);
	}
	
	// Unmap shared memory from process' address space.
	munmap(table, sizeof(table_t));
	table = NULL;
	
	return 0;
}
