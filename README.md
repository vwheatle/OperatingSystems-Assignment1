V Wheatley  
Operating Systems  
Assignment 1

# Producer-Consumer Problem

This consists of two C programs -- one a Producer, and the other a Consumer. The Producer creates... *things*, and the Consumer consumes them. They're both separate programs that'll communicate.

How will I implement this? We'll find out, I guess.

Here's a reinterpretation of the requirements document, in Markdown.

> ## Requirements
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
> ## Hints
>
> - Shared Memory
>   - You need shared memory to build the buffer accessible by two processes.
>   - Yes, two processes.
>   - [The `man` page for `shm_open`](https://man7.org/linux/man-pages/man3/shm_open.3.html).
> - Mutual Exclusion
>   - Simply use [a Semaphore](https://www.geeksforgeeks.org/use-posix-semaphores-c/).
>
> Good luck!

## What I'm using

I'm&zwnj;~~... not~~ using C in Linux! I'm using ~~MSYS2~~ *WSL2* to build and run these programs. ~~Despite this,~~ I'm ~~still~~ using bash, ~~still~~ using gcc, ~~still~~ using pthreads. It ~~should~~ work ~~the exact same~~ on Linux.

Run `build.sh` and `cd` into `bin`. Then you can do the simultaneous thing in the `bin` folder.

## Web resources too

- https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html oh thank god for this
