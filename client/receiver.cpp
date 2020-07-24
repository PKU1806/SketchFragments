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
	if (argc != 3) {
		perror("args error.");
		exit(1);
	}

	Receiver::Receiver receiver(atoi(argv[1]));
	receiver.receive(atoi(argv[2]));

	return 0;
}
