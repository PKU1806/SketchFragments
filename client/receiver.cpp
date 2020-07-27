#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <chrono>
#include <iostream>

#include "header.h"
#include "receiver.h"

const int sw_num = 10;
const int ar_num = 3;
const int bu_num = 64;

int main(int argc, char **argv) {
	if (argc != 4) {
		perror("args error.");
		exit(1);
	}

	Simulator::Aggregator agg(sw_num, ar_num, bu_num);

	Simulator::Receiver receiver(atoi(argv[1]), agg, argv[3]);

	receiver.receive(atoi(argv[2]));

	return 0;
}
