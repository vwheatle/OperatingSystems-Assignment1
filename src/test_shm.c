#include <stdio.h>  // -> printf
#include <stdlib.h> // -> exit, etc.
#include <unistd.h> // -> sleep

#include <errno.h> // -> errno

#include <sys/types.h> // -> types for shared memory
#include <sys/mman.h>  // -> shared memory
#include <sys/stat.h>  // -> mode constants; fstat
#include <fcntl.h>     // -> O_* constants

#include <pthread.h>   // -> pthreads
#include <semaphore.h> // -> semaphores

#define TRUE 1
#define FALSE 0

#define theShmName "/testingtheshm"

typedef struct {
	sem_t mySem;
	int danger;
} shm_struct;

int getSharedMemSize(int fd) {
	struct stat stats;
	fstat(fd, &stats);
	return stats.st_size;
}

void initSharedMem(shm_struct* s) {
	printf("Initializing shared mem...\n");
	sem_init(&s->mySem, 1, 1);
	s->danger = 0;
}

int main() {
	int wasThatGood = FALSE;
	
	// TODO: switch this over to O_EXCL so that
	// no procs get telefragged or whatever
	int fd = shm_open(theShmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		perror("ERROR!! shm_open returned error");
		exit(EXIT_FAILURE);
	}
	printf("file descriptor: %d\n", fd);
	
	int shSize = getSharedMemSize(fd);
	printf("shm size: %d\n", shSize);
	
	if (ftruncate(fd, sizeof(shm_struct)) < 0) {
		perror("ERROR!! Truncate failed");
		goto die;
	}
	
	printf("shm size after: %d\n", getSharedMemSize(fd));
	
	shm_struct* sharing = mmap(
		NULL, sizeof(shm_struct), // address & size
		PROT_READ | PROT_WRITE,   // page permissions
		MAP_SHARED, fd, 0         // other stuff
	);
	if (sharing == MAP_FAILED) {
		perror("ERROR!! Map failed");
		goto die;
	}
	
	printf("%x\n", sharing);
	
	if (shSize == 0) {
		printf("Gotta initialize it...\n");
		initSharedMem(sharing);
		printf("Okay...\n");
	} else {
		printf("Someone already initialized it maybe?\n");
	}
	
	printf("Waiting on lock...\n");
	sem_wait(&sharing->mySem);
	
	printf("Inside critical section. %d is now... ", sharing->danger);
	sharing->danger++;
	printf("%d!!!\n", sharing->danger);
	
	printf("Unlocking\n");
	sem_post(&sharing->mySem);
	
	wasThatGood = TRUE;
	printf("I think that's all I wanted to do?\n");
die:
	shm_unlink(theShmName);
	
	exit(wasThatGood ? EXIT_SUCCESS : EXIT_FAILURE);
}
