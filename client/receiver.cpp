#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "header.h"

#define RECV_PORT 8000

int main() {
	int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock_fd < 0) {
		perror("socket.");
		exit(1);
	}

	struct sockaddr_in addr_send;
	struct sockaddr_in addr_recv;
	int addr_len = sizeof(addr_recv);
	memset(&addr_send, 0, addr_len);
	memset(&addr_recv, 0, addr_len);


	SF_Header sf_header;
	int header_len = sizeof(sf_header);
	memset((char *)&sf_header, 0x00, header_len);

	addr_recv.sin_family = AF_INET;
	addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_recv.sin_port = htons(RECV_PORT);

	if (bind(sock_fd, (struct sockaddr *)&addr_recv, addr_len) < 0) {
		perror("bind error.");
		exit(1);
	}

	int recv_num;
	while (true) {
		printf("recver wait.\n");

		recv_num = recvfrom(sock_fd, (char *)&sf_header, header_len, 0,
			   	(struct sockaddr *)&addr_send, (socklen_t *)&addr_len);

		if (recv_num < 0) {
			perror("recvfrom error.");
			exit(1);
		}

		sf_header.mih_switch_id = be16toh(sf_header.mih_switch_id);
		sf_header.mih_timestamp = be64toh(sf_header.mih_timestamp);
		sf_header.sfh_switch_id = be16toh(sf_header.sfh_switch_id);
		sf_header.sfh_fgment_id = be32toh(sf_header.sfh_fgment_id);

		sf_header.sfh_delay0 = be32toh(sf_header.sfh_delay0);
		sf_header.sfh_delay1 = be32toh(sf_header.sfh_delay1);
		sf_header.sfh_delay2 = be32toh(sf_header.sfh_delay2);
		sf_header.sfh_delay3 = be32toh(sf_header.sfh_delay3);
		sf_header.sfh_delay4 = be32toh(sf_header.sfh_delay4);
		sf_header.sfh_delay5 = be32toh(sf_header.sfh_delay5);
		sf_header.sfh_delay6 = be32toh(sf_header.sfh_delay6);
		sf_header.sfh_delay7 = be32toh(sf_header.sfh_delay7);
		sf_header.sfh_delay8 = be32toh(sf_header.sfh_delay8);
		sf_header.sfh_delay9 = be32toh(sf_header.sfh_delay9);

		if (sf_header.sfh_switch_id != 0) {
			printf("mih_switch_id : %u\n" , sf_header.mih_switch_id);
			printf("mih_timestamp : %lu\n", sf_header.mih_timestamp);
			printf("sfh_switch_id : %u\n" , sf_header.sfh_switch_id);
			printf("sfh_sketch_fg : %u\n" , sf_header.sfh_sketch_fg);
			printf("sfh_fgment_id : %u\n" , sf_header.sfh_fgment_id);

			printf("sketch fragment :\n");

			printf("sfh_delay0 : %u\n", sf_header.sfh_delay0);
			printf("sfh_delay1 : %u\n", sf_header.sfh_delay1);
			printf("sfh_delay2 : %u\n", sf_header.sfh_delay2);
			printf("sfh_delay3 : %u\n", sf_header.sfh_delay3);
			printf("sfh_delay4 : %u\n", sf_header.sfh_delay4);
			printf("sfh_delay5 : %u\n", sf_header.sfh_delay5);
			printf("sfh_delay6 : %u\n", sf_header.sfh_delay6);
			printf("sfh_delay7 : %u\n", sf_header.sfh_delay7);
			printf("sfh_delay8 : %u\n", sf_header.sfh_delay8);
			printf("sfh_delay9 : %u\n", sf_header.sfh_delay9);
		}
	}

	close(sock_fd);

	return 0;
}
