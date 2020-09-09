#!/bin/bash

for ((i = 1; i <= 16; i++))
do
for ((j = 1; j <= 16; j++))
do
	if ((i != j))
       	then
		echo h$i h$j 1000
	fi
done
done
