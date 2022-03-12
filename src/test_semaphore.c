#include <stdio.h>  // -> printf
#include <stdlib.h> // -> exit, etc.
#include <unistd.h> // -> sleep

#include <pthread.h>   // -> pthreads
#include <semaphore.h> // -> semaphores

#define NUM_THREADS 4
sem_t thingSem;

void* do_thing(void* args) {
	// Get thread identifier
	pthread_t me = pthread_self();
	
	// I don't know, this is just for the colors to be different enough.
	unsigned long int me2 = me >> 11; // strip out trailing zeroes
	int color = (me2 & 127) + 16; // keep only the low 7 bits and add 16 to make a valid color value
	
	printf("\x1B[38;5;%dm [%lx] waiting for a turn on the video game\x1B[0m\n", color, me);
	
	// Try to take 1 from the semaphore.
	sem_wait(&thingSem);
	
	printf("\x1B[38;5;%dm [%lx] gaming\x1B[0m\n", color, me);
	usleep(250);
	
	// Put 1 back into the semaphore.
	sem_post(&thingSem);
	
	printf("\x1B[38;5;%dm [%lx] done gaming!\x1B[0m\n", color, me);
}

int main() {
	// Creates the semaphore with an initial value of 1.
	// Change that third argument to change the amount of gamers allowed at a time.
	sem_init(&thingSem, 0, 1);
	
	pthread_t threads[NUM_THREADS];
	
	for (int i = 0; i < NUM_THREADS; i++) {
		// Make some threads.
		pthread_create(&threads[i], NULL, do_thing, NULL);
		usleep(100);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		// Wait for all of them to complete their task.
		pthread_join(threads[i], NULL);
	}
	
	// Destroy the semaphore mercilessly.
	sem_destroy(&thingSem);
	
	exit(0);
}

// Built with:
// gcc test_semaphore.c -pthread -o test_semaphore && ./test_semaphore
