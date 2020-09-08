#!/bin/bash

for ((i = 1; i <= 8; i++))
do
for ((j = 1; j <= 8; j++))
do
	if ((i != j))
       	then
		echo h$i h$j 100
	fi
done
done
