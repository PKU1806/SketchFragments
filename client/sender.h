#ifndef __SENDER_H__
#define __SENDER_H__

#include "header.h"
#include "host.h" 

namespace Simulator {

struct Sender : Host {
	std::string recv_ip;

	sockaddr_in addr_recv;
	MIH_Header mih_header;

	int recv_port, sock_fd, addr_len, header_len;

	Sender(std::string ip, int port, std::string pid) : Host(pid) {

		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock_fd < 0) {
			perror("socket error.");
			exit(1);
		}

		recv_ip = ip;
		recv_port = port;

		printf("ip: %s, port: %d\n", recv_ip.c_str(), recv_port);

		addr_len = sizeof(addr_recv);
		header_len = sizeof(mih_header);

		memset((char *)&addr_recv, 0x00, addr_len);
		memset((char *)&mih_header, 0x00, header_len);

		addr_recv.sin_family = AF_INET;
		addr_recv.sin_addr.s_addr = inet_addr(recv_ip.c_str());
		addr_recv.sin_port = htons(recv_port);

		mih_header.exists_fg = 0;
	}

	void send(int maxp) {
		for (int sendp = 0; sendp < maxp || maxp < 0; sendp++) {
			int send_num = sendto(sock_fd, (char *)&mih_header, header_len,
				   	0, (sockaddr *)&addr_recv, addr_len);

			// printf("send: %d / %d bytes (PKT : %d)\n", send_num, header_len, sendp);
			// printf("recv ip: %s, recv port: %d\n", recv_ip.c_str(), recv_port);

			if (send_num < 0) {
				perror("sendto error.");
				exit(1);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	void switch_namespace(std::string pid) {
		char path[MAX_PATH_LENGTH];

		sprintf(path, "/proc/%s/ns/net", pid.c_str());
		printf("attach net: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/pid", pid.c_str());
		printf("attach pid: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/mnt", pid.c_str());
		printf("attach mnt: %d.\n", attachToNS(path));

		if(system("ifconfig | grep inet") != 0) {
			exit(0);
		}
	}

	int attachToNS(char *path) {
		int nsid = open(path, O_RDONLY);
		return setns(nsid, 0);
	}

	~Sender() {
		close(sock_fd);
	}
};
}

#endif
