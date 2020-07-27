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
	if (argc != 4) {
		perror("args error.");
		exit(1);
	}

	Simulator::Sender sender(std::string(argv[1]), atoi(argv[2]));

	char path[MAX_PATH_LENGTH];

	sprintf(path, "/proc/7994/ns/net");
	std::cout << path << std::endl;
	printf("attach net: %d.\n", attachToNS(path));

	sprintf(path, "/proc/7994/ns/pid");
	std::cout << path << std::endl;
	printf("attach pid: %d.\n", attachToNS(path));

	sprintf(path, "/proc/7994/ns/mnt");
	std::cout << path << std::endl;
	printf("attach mnt: %d.\n", attachToNS(path));

	system("ifconfig | grep inet");

	sender.send(atoi(argv[3]));

	return 0;
}
