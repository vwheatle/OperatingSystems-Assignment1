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

mkdir bin
echo "producer" ; gcc src/producer.c -pthread -lrt -o out/producer
echo "consumer" ; gcc src/consumer.c -pthread -lrt -o out/consumer
