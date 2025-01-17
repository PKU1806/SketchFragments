#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "header.h"
#include "sender.h"

int attachToNS(char *path) {
	int nsid = open(path, O_RDONLY);
	return setns(nsid, 0);
}

int main(int argc, char **argv) {
	if (argc != 5) {
		perror("args error.");
		exit(1);
	}

	Simulator::Sender sender(std::string(argv[1]), atoi(argv[2]), argv[4]);

	sender.send(atoi(argv[3]));

	return 0;
}
