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

echo "producer" ; gcc src/producer.c -pthread -o bin/producer
echo "consumer" ; gcc src/consumer.c -pthread -o bin/consumer
