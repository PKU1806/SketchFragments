#ifndef __HOST_H__
#define __HOST_H__

#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include <sched.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "header.h"

namespace Simulator {

struct Host {
	Host(std::string pid) {
		switch_namespace(pid);
	}

	void switch_namespace(std::string pid) {
		char path[MAX_PATH_LENGTH];

		sprintf(path, "/proc/%s/ns/net", pid.c_str());
		printf("attach net: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/pid", pid.c_str());
		printf("attach pid: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/mnt", pid.c_str());
		printf("attach mnt: %d.\n", attachToNS(path));

		if (system("ifconfig | grep inet") != 0) {
			exit(0);
		}
	}

	int attachToNS(char *path) {
		int nsid = open(path, O_RDONLY);
		return setns(nsid, 0);
	}

};

}

#endif
