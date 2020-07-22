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

#define RECV_PORT 8000
#define RECV_IP_ADDRESS "10.6.2.2"

int main(int argc, char **argv) {
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (argc != 2) {
		perror("args error.");
		exit(1);
	}

	if (sock_fd < 0) {
		perror("socket error.");
		exit(1);
	}

	struct sockaddr_in addr_recv;
	int addr_len = sizeof(addr_recv);
	memset(&addr_recv, 0, addr_len);

	MIH_Header mih_header;
	int header_len = sizeof(mih_header);
	memset((char *)&mih_header, 0x00, header_len);

	mih_header.exists_fg = 0;

	addr_recv.sin_family = AF_INET;
	addr_recv.sin_addr.s_addr = inet_addr(RECV_IP_ADDRESS);
	addr_recv.sin_port = htons(RECV_PORT);

	int send_pkt = 0;
	int send_num;
	for(auto _ = 0; _ < atoi(argv[1]); _++) {
		send_num = sendto(sock_fd, (char *)&mih_header, header_len, 0,
				(struct sockaddr *)&addr_recv, addr_len);

		printf("send: %d / %d bytes (PKT : %d)\n", send_num, header_len,
				++send_pkt);

		if (send_num < 0) {
			perror("sendto error.");
			exit(1);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	close(sock_fd);

	return 0;
}
