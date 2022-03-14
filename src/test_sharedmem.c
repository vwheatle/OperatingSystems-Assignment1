#include <stdio.h>   // -> printf
#include <stdlib.h>  // -> exit, etc.
#include <stdbool.h> // -> booleans?! (surprised)
#include <unistd.h>  // -> sleep, getpid

#include <errno.h> // -> errno

#include <sys/types.h> // -> types for shared memory
#include <sys/mman.h>  // -> shared memory
#include <sys/stat.h>  // -> mode constants; fstat
#include <fcntl.h>     // -> O_* constants

#include <pthread.h>   // -> pthreads
#include <semaphore.h> // -> semaphores

// Really bad macro I'm going to use extensively anyway.
// https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
// This might be GNU C only?
pid_t me;
int colorFromPid(pid_t p) { // i don't like this, but it kinda works
	int pt = p & 7; return pt >= 4 ? (pt - 4 + 91) : (pt + 31);
}
#define printf_withme(format, ...) printf("\033[%dm[%d]\033[0m " format, colorFromPid(me), me __VA_OPT__(,) __VA_ARGS__);
#define perror_withme(message) printf_withme("!!! "); perror(message)

#define theShmName "/testingshm"

typedef struct {
	sem_t mySem;
	int procCount;
	int dangerousCounter;
	bool isReady;
} shm_struct;

int getSharedMemSize(int fd) {
	struct stat stats;
	fstat(fd, &stats);
	return stats.st_size;
}

void initSharedMem(shm_struct* s) {
	printf_withme("Initializing shared mem...\n");
	
	sem_init(&s->mySem, 1, 1);
	s->procCount = 0;
	s->dangerousCounter = 0;
	
	s->isReady = true;
}

int main() {
	bool wasThatGood = false;
	
	me = getpid();
	bool shouldInitialize = false;
	
	int fd = shm_open(theShmName, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		if (errno == EEXIST) {
			printf_withme("Not in charge of shared memory initialization.\n");
			
			fd = shm_open(theShmName, O_RDWR, S_IRUSR | S_IWUSR);
			if (fd < 0) {
				perror_withme("Failed to open shared memory");
				exit(EXIT_FAILURE);
			}
		} else {
			perror_withme("Failed to open shared memory");
			exit(EXIT_FAILURE);
		}
	} else {
		shouldInitialize = true;
		printf_withme("In charge of shared memory initialization.\n");
	}
	
	// Neither resizing nor mapping are critical sections, so
	// all processes can do them without much thought. However...
	
	// Shared memory starts at 0 bytes.
	// Resize it to the actual amount needed.
	// The resized portion will be filled with zeroes,
	// so I take advantage of that.
	if (ftruncate(fd, sizeof(shm_struct)) < 0) {
		perror_withme("Extending shared memory failed");
		goto die;
	}
	
	// Map the shared memory into this program's addr. space.
	// This returns a pointer to what is essentially our struct.
	shm_struct* sharing = mmap(
		NULL, sizeof(shm_struct), // address & size
		PROT_READ | PROT_WRITE,   // page permissions
		MAP_SHARED, fd, 0         // other stuff
	);
	if (sharing == MAP_FAILED) {
		perror_withme("Memory mapping failed");
		goto die;
	}
	
	// The first instance to run should initialize the shared memory.
	if (shouldInitialize) {
		initSharedMem(sharing);
		printf_withme("Done initializing.\n");
	} else {
		printf_withme("Probably initialized. Waiting for flag...\n");
		while (!sharing->isReady) usleep(100);
		// I think this is guaranteed to work?
		// Someone correct me if I'm wrong.
	}
	
	// Everything should be good to go after this part.
	
	// I think it'd be fun to have "total procs working together at once".
	sem_wait(&sharing->mySem);
	sharing->procCount++;
	printf_withme("Looks like I'm the %d proc...\n", sharing->procCount);
	sem_post(&sharing->mySem);
	
	// And now they all work together to increment a thing!
	for (int i = 0; i < 8; i++) {
		printf_withme("Awaiting lock...\n");
		sem_wait(&sharing->mySem);
		
		printf_withme("Locked. %d is now ", sharing->dangerousCounter);
		sharing->dangerousCounter++;
		printf("%d!\n", sharing->dangerousCounter);
		
		printf_withme("Unlocking.\n");
		sem_post(&sharing->mySem);
		
		printf_withme("Sleeping for a bit...\n");
		usleep(rand() % 100 * 10000);
	}
	
	wasThatGood = true;
	printf_withme("Done, unlinking and quitting.\n");
die:
	// "shm_unlink() [...] removes a shared memory object name, and,
	// **once all processes have unmapped the object**, deallocates
	// and destroys the contents of the associated memory region."
	// src: https://man7.org/linux/man-pages/man3/shm_open.3.html
	// This doesn't destroy the shared mem until all procs stop using it.
	shm_unlink(theShmName);
	
	exit(wasThatGood ? EXIT_SUCCESS : EXIT_FAILURE);
}
