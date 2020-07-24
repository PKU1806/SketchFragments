#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "header.h"
#include "sender.h"

int main(int argc, char **argv) {
	if (argc != 4) {
		perror("args error.");
		exit(1);
	}

	Sender sender(argv[1], atoi(argv[2]));

	sender.send(atoi(argv[3]));

	return 0;
}
