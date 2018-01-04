#!/bin/bash

rm log.txt
g++ -g -o test_main test_main.cc cluster.cc grid.cc -std=c++11 > log.txt 2>&1
