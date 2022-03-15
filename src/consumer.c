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

#include "shared_memory.h"

#define PROC_LABEL_STR "\033[96mConsumer\033[0m: "

#define printf_label(...) printf(PROC_LABEL_STR); printf(__VA_ARGS__)
#define perror_label(message) printf(PROC_LABEL_STR); perror(message)

#define perror_exit(message) do { perror_label(message); exit(EXIT_FAILURE) } while (0)
// do {} while (0) used in man pages, explained https://stackoverflow.com/a/9496007/

void meat(table_t* table) {
	do {
		sem_wait(&table->full);
		sem_wait(&table->mutex);
		
		// Take from the buffer.
		
		sem_post(&table->mutex);
		sem_post(&table->empty);
		
		// Consume something.
	} while (true);
}

int main() {
	printf_label("Hello, world!\n");
	
	// TODO: no body...
	
	exit(EXIT_SUCCESS);
}
