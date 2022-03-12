#!/bin/bash
set -eu

echo "Cleaning..."

echo "Possible Windows-style executables..."
echo "consumer.exe     "; rm -f ./consumer.exe
echo "producer.exe     "; rm -f ./producer.exe

echo "Possible Linux-style executables..."
echo "consumer     "; rm -f ./consumer
echo "producer     "; rm -f ./producer
