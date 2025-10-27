#!/bin/bash
set -eu

# Compile the programs with GCC
# -pthread : enables POSIX threads
# -lrt     : links the rt (runtime?) library
# -o name  : outputs an executable named name

# Looking here, it seems librt is now part of libc?
# https://docs.oracle.com/cd/E86824_01/html/E54772/librt-3lib.html
# So I might remove that.

echo "Building..."

mkdir -p out

echo "The actual stuff you wanted:"

echo "producer" ; gcc ./src/producer.c -pthread -lrt -Wall -Wunused -Wuninitialized -o ./out/producer
echo "consumer" ; gcc ./src/consumer.c -pthread -lrt -Wall -Wunused -Wuninitialized -o ./out/consumer

echo "The testing grounds stuff:"

echo "test_race_cond" ; gcc ./src/test_race_cond.c -pthread -lrt -Wall -Wunused -Wuninitialized -o ./out/test_race_cond
echo "test_semaphore" ; gcc ./src/test_semaphore.c -pthread -lrt -Wall -Wunused -Wuninitialized -o ./out/test_semaphore
echo "test_sharedmem" ; gcc ./src/test_sharedmem.c -pthread -lrt -Wall -Wunused -Wuninitialized -o ./out/test_sharedmem
