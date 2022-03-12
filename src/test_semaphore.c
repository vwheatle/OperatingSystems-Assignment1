#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 16
sem_t thingSem;

void* do_thing(void* args) {
	pthread_t me = pthread_self();
	unsigned long int me2 = me >> 11;
	int color = (me2 & 127) + 16;
	
	printf("\x1B[38;5;%dm [%lx] waiting for a turn on the video game\x1B[0m\n", color, me);
	
	sem_wait(&thingSem);
	
	printf("\x1B[38;5;%dm [%lx] gaming\x1B[0m\n", color, me);
	usleep(1000);
	
	sem_post(&thingSem);
	
	printf("\x1B[38;5;%dm [%lx] done gaming!\x1B[0m\n", color, me);
}

int main() {
	// Creates the semaphore with an initial value of 1.
	sem_init(&thingSem, 0, 1);
	
	// Profess my love
	printf("i <3 semaphore\n");
	
	pthread_t threads[NUM_THREADS];
	
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, do_thing, NULL);
		usleep(100);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// Goodbye...
	sem_destroy(&thingSem);
	
	exit(0);
}
