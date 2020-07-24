#!/bin/bash

host_num=8

cat /dev/null > sender.config
cat /dev/null > receiver.config

for host in $(seq 1 $host_num)
do
	echo h$host >> sender.config
	ps au | grep mininet | grep h$host | awk '{print $2}' >> sender.config
done

for host in $(seq 1 $host_num)
do
	echo h$host >> receiver.config
	mx h$host ifconfig | grep inet | grep -v 127.0.0.1 | awk '{print $2}' | tr -d "addr:" >> receiver.config
done
