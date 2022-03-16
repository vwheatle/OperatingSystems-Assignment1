#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include <pthread.h>

#define NUM_THREADS 128
#define SCRAMBLE_LEN 128
pthread_mutex_t thingMutex = PTHREAD_MUTEX_INITIALIZER;
char thingsOrig[] = "The quick brown fox jumped over the lazy dog. Colorless green ideas sleep furiously. More people have been inside the CS building than I have.";
char things[] =     "The quick brown fox jumped over the lazy dog. Colorless green ideas sleep furiously. More people have been inside the CS building than I have.";

void* thread_wrong_fn(void* arg) {
	// int me = pthread_self();
	
	for (int i = 0; i < SCRAMBLE_LEN; i++) {
		things[i] = ((things[(SCRAMBLE_LEN - 1) - i] - 0x30) % 0x40) + 0x31;
		usleep(10 + (rand() & 7));
	}
	
	// printf("%ld incs by 10; thing is now %d\n", me, thing);
	
	return NULL;
}

void* thread_right_fn(void* arg) {
	pthread_mutex_lock(&thingMutex);
	
	for (int i = 0; i < SCRAMBLE_LEN; i++) {
		things[i] = ((things[(SCRAMBLE_LEN - 1) - i] - 0x30) % 0x40) + 0x31;
		usleep(5);
	}
	
	pthread_mutex_unlock(&thingMutex);
	
	return NULL;
}

int main() {
	pthread_t threads[NUM_THREADS];
	
	for (int does = 0; does < 4; does++) {
		printf("Oops version iter %d\n", does);
		memcpy(things, thingsOrig, sizeof(thingsOrig) * sizeof(thingsOrig[0]));
		printf("Before everything, thing was   %s\n", things);
		
		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_create(&threads[i], NULL, thread_wrong_fn, NULL);
		}
		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(threads[i], NULL);
		}
		
		printf("After everything, thing became %s, with %d threads.\n\n", things, NUM_THREADS);
	}
	
	printf("Mutex version\n");
	memcpy(things, thingsOrig, sizeof(thingsOrig) * sizeof(thingsOrig[0]));
	printf("Before everything, thing was   %s\n", things);
	
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, thread_right_fn, NULL);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	printf("After everything, thing became %s, with %d threads.\n\n", things, NUM_THREADS);
	
	printf("Non-thread version\n");
	memcpy(things, thingsOrig, sizeof(thingsOrig) * sizeof(thingsOrig[0]));
	printf("Before everything, thing was   %s\n", things);
	
	for (int i = 0; i < NUM_THREADS; i++) {
		thread_right_fn(NULL);
	}
	
	printf("After everything, thing became %s.\n\n", things);
	
	exit(0);
}
