V Wheatley  
Operating Systems  
Assignment 1

# Producer-Consumer Problem

This consists of two C programs -- one a Producer, and the other a Consumer. The Producer creates integers, and the Consumer consumes them. They're both separate programs that communicate using POSIX shared memory.

![A screenshot of the producer and consumer programs in action, in both Linux (left) and Windows, using WSL2 (right)](.readme/linuxAndWSL2.png)

## Project Layout

The `src` folder contains all the source code.

- `producer.c` & `consumer.c` - There they are!
- `process_sync.h` - Utility functions for setting up the shared-memory "Table".
- `shared_memory.h` - The Table itself.
- `test_*.c` - Some prototypes I wrote to build understanding of POSIX shared memory objects and semaphores.

There are also two basic build scripts.

- `build.sh` - Builds the producer & consumer binaries along with the prototypes, and puts them all in an `out` folder.
- `clean.sh` - Destroys the `out` folder.

## Requirements

Here's a reinterpretation of the requirements document, in Markdown.

> ### Requirements
> 
> In this programming assignment, you must provide the source codes, documents, and examples in detail on Github. They will be counted for the final score. Please submit the link to the project on Github.
> 
> Requirements:
> - Use C/C++ in Linux. Consider installing VirtualBox if you don't use Linux.
> - Programs should work. (80 points)
> - Readme, Documentation, and Examples are required. (20 Points)
> 
> The producer generates items and puts items onto the table. The consumer will pick up items. The table can only hold two items at the same time. When the table is complete, the producer will wait. When there are no items, the consumer will wait. We use semaphores to synchronize producer and consumer. Mutual exclusion should be considered. We use threads in the producer program and consumer program. Shared memory is used for the “table”.
> 
> Once built, we expect to use these two programs (producer and consumer) like this:
> 
> ```shell
> $ ./producer & ./consumer &
> ```
>
> ### Hints
>
> - Shared Memory
>   - You need shared memory to build the buffer accessible by two processes.
>   - Yes, two processes.
>   - [The `man` page for `shm_open`](https://man7.org/linux/man-pages/man3/shm_open.3.html).
> - Mutual Exclusion
>   - Simply use [a Semaphore](https://www.geeksforgeeks.org/use-posix-semaphores-c/).
>
> Good luck!

## What I'm Using

<!-- I'm&zwnj;~~... not~~ using C in Linux! I'm using ~~MSYS2~~ *WSL2* to build and run these programs. ~~Despite this,~~ I'm ~~still~~ using bash, ~~still~~ using gcc, ~~still~~ using pthreads. It ~~should~~ work ~~the exact same~~ on Linux. -->

I'm using C in Linux! I'm using [*WSL2*](https://docs.microsoft.com/en-us/windows/wsl/) to build and run these programs. I'm using bash, using gcc, using POSIX shared memory. It work on Linux. [sic]

Run `build.sh`. It will create the `out` folder, which will contain the binaries. Simply do...

```shell
./producer & ./consumer & sleep 2s
```

...and it should output something like the following:

```shell
[1] pid1
[2] pid2
[95mProducer[0m: Hello, world!
[96mConsumer[0m: Hello, world!
[95mProducer[0m: In charge of shared memory initialization.
[95mProducer[0m: Done initializing.
[96mConsumer[0m: Not in charge of shared memory initialization.
[95mProducer[0m: Starting.
[95mProducer[0m: Made 103.
[96mConsumer[0m: Waiting for ready flag...
[95mProducer[0m: Made 198.
[95mProducer[0m: Made 105.
[96mConsumer[0m: Starting.
[96mConsumer[0m: Ate 103.
...
[96mConsumer[0m: Ate 242.
[95mProducer[0m: Made 70.
[95mProducer[0m: Stopping.
[96mConsumer[0m: Ate 251.
[96mConsumer[0m: Ate 227.
[96mConsumer[0m: Ate 70.
[96mConsumer[0m: Stopping.
[96mConsumer[0m: Cleaning up.
[1]-  Done                    ./producer
[2]+  Done                    ./consumer
```

The numbers and order will be random, but there will be the same amount of corresponding `Made %d` and `Ate %d` messages.

## Resources Used

- I extensively used the man pages `sem_overview` and `shm_overview`.

<!--## Web resources too

- https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html oh thank god for this
- https://man7.org/linux/man-pages/man0/unistd.h.0p.html
- https://man7.org/linux/man-pages/man0/pthread.h.0p.html -->
