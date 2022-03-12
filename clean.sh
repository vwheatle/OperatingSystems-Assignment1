#!/bin/bash
set -eu

echo "Cleaning..."

echo "producer" ; rm -f ./bin/producer ./bin/producer.exe
echo "consumer" ; rm -f ./bin/consumer ./bin/consumer.exe
