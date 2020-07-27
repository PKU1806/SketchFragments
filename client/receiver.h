#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include <sched.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "header.h"
#include "aggregator.h"

namespace Simulator {

struct Receiver {
	sockaddr_in addr_send, addr_recv;
	COM_Header com_header;

	int sock_fd, recv_port, addr_len, header_len;

	Aggregator &aggregator;

	Receiver(int port, Aggregator agg) : aggregator(agg) {
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock_fd < 0) {
			perror("socket.");
			exit(1);
		}

		recv_port = port;

		printf("port: %d.\n", recv_port);

		addr_len = sizeof(addr_recv);
		header_len = sizeof(com_header);

		memset((char *)&addr_send, 0x00, addr_len);
		memset((char *)&addr_recv, 0x00, addr_len);
		memset((char *)&com_header, 0x00, header_len);

		addr_recv.sin_family = AF_INET;
		addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
		addr_recv.sin_port = htons(recv_port);

		if (bind(sock_fd, (sockaddr *)&addr_recv, addr_len) < 0) {
			perror("bind error.");
			exit(1);
		}
	}

	void receive(int maxp) {
		for (int recvp = 0; recvp < maxp || maxp < 0; recvp++) {
			int recv_num = recvfrom(sock_fd, (char *)&com_header, header_len,
				   	0, (sockaddr *)&addr_send, (socklen_t *)&addr_len);

			printf("receive: %d / %d bytes (PKT : %d, FG : %d)\n", recv_num,
				   	header_len, recvp, com_header.mih.exists_fg);
			printf("send ip: %s, send port: %d.\n",
				   	inet_ntoa(addr_send.sin_addr), addr_send.sin_port);

			if (recv_num < 0) {
				perror("recvfrom error.");
				exit(1);
			}

			betoh_header(com_header);
			if (com_header.mih.exists_fg != 0) {
				aggregator.receive_header(com_header);
			}
		}
	}

	void betoh_header(COM_Header &com_header) {
		com_header.mih.switch_id = be16toh(com_header.mih.switch_id);
		com_header.mih.tim_epoch = be64toh(com_header.mih.tim_epoch);
		com_header.sfh.switch_id = be16toh(com_header.sfh.switch_id);
		com_header.sfh.fgment_id = be32toh(com_header.sfh.fgment_id);

		for (int i = 0; i < 10; i++) {
			com_header.sfh.delay[i] = be32toh(com_header.sfh.delay[i]);
		}
	}

	void switch_namespace(std::string pid) {
		char path[MAX_PATH_LENGTH];

		sprintf(path, "/proc/%s/ns/net", pid.c_str());
		std::cout << path << std::endl;
		printf("attach net: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/pid", pid.c_str());
		std::cout << path << std::endl;
		printf("attach pid: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/mnt", pid.c_str());
		std::cout << path << std::endl;
		printf("attach mnt: %d.\n", attachToNS(path));

		system("ifconfig | grep inet");
	}

	int attachToNS(char *path) {
		int nsid = open(path, O_RDONLY);
		return setns(nsid, 0);
	}

	~Receiver() {
		close(sock_fd);
	}
};
}

#endif
