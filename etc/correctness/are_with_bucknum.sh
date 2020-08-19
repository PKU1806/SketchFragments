#!/bin/bash

g++ -DChangeBuckNum --std=c++11 are_with_bucknum.cpp -o are_with_bucknum.out

./are_with_bucknum.out -a 1 -b 50 -c 1 &
./are_with_bucknum.out -a 50 -b 100 -c 1 &
./are_with_bucknum.out -a 100 -b 500 -c 10 &
./are_with_bucknum.out -a 500 -b 1000 -c 10 &
./are_with_bucknum.out -a 1000 -b 10000 -c 100 &
./are_with_bucknum.out -a 10000 -b 20000 -c 100 &
./are_with_bucknum.out -a 20000 -b 30000 -c 100 &
./are_with_bucknum.out -a 30000 -b 40000 -c 100 &
./are_with_bucknum.out -a 40000 -b 50000 -c 100 &