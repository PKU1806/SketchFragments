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

int main(int argc, char **argv) {
	if (argc != 4) {
		perror("args error.");
		exit(1);
	}

	Simulator::Aggregator agg(Simulator::sw_num, Simulator::ar_num,
		   	Simulator::bu_num);

	Simulator::Receiver receiver(atoi(argv[1]), &agg, argv[3]);

	receiver.receive(atoi(argv[2]));

	return 0;
}
