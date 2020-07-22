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

#define RECV_PORT 8000


const int SWITCH_NUM = 6;
const int ARRAY_NUM = 3;
const int BUCKET_NUM = 16;

COM_Header sketch[SWITCH_NUM][ARRAY_NUM][BUCKET_NUM];

int sketch_flag[SWITCH_NUM], sketch_fnum[SWITCH_NUM];

int packet_num, packet_start[SWITCH_NUM], packet_epoch[SWITCH_NUM];

std::chrono::steady_clock::time_point time_start[SWITCH_NUM];
std::chrono::milliseconds time_epoch[SWITCH_NUM];

void visor_sketch(int switch_id, int sketch_fg, std::chrono::milliseconds time_epoch) {
	std::cout << "switch " << switch_id + 1 << " recieve sketch " << sketch_fg << 
		" using " << time_epoch.count() << " milliseconds." << std::endl;

	for (int i = 0; i < ARRAY_NUM; i++) {
		printf("sketch array %d.\n", i);

		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < BUCKET_NUM; k++) {
				printf("|%3u|", sketch[switch_id][i][k].sfh.delay[j]);
			}
			printf("\n");
		}
	}
}

void receive_fragment(COM_Header &com_header) {
	int switch_id = com_header.sfh.switch_id - 8001;
	int sketch_fg = com_header.sfh.sketch_fg;
	int array_id = com_header.sfh.fgment_id / BUCKET_NUM;
	int bucket_id = com_header.sfh.fgment_id % BUCKET_NUM;

	if (sketch_flag[switch_id] != sketch_fg) {
		printf("switch %d detect sketch %d.\n", switch_id + 1, sketch_fg);

		sketch_flag[switch_id] = sketch_fg;
		sketch_fnum[switch_id] = 0;

		memset(sketch[switch_id], 0x00, ARRAY_NUM * BUCKET_NUM * sizeof(COM_Header));

		time_start[switch_id] = std::chrono::steady_clock::now();
	}

	if (sketch[switch_id][array_id][bucket_id].sfh.switch_id == 0) {
		printf("switch %d detect new fragment %d.\n", switch_id + 1, com_header.sfh.fgment_id);

		sketch[switch_id][array_id][bucket_id] = com_header;
		sketch_fnum[switch_id] += 1;

		if (sketch_fnum[switch_id] == BUCKET_NUM * ARRAY_NUM) {
			auto time_end = std::chrono::steady_clock::now();
			time_epoch[switch_id] = std::chrono::duration_cast<std::chrono::milliseconds>
				(time_end - time_start[switch_id]);
			visor_sketch(switch_id, sketch_fg, time_epoch[switch_id]);
		}
	}
}

int main() {
	memset(sketch_flag, 0xFF, sizeof(sketch_flag));
	memset(sketch_fnum, 0x00, sizeof(sketch_fnum));

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


	COM_Header com_header;
	int header_len = sizeof(com_header);
	memset((char *)&com_header, 0x00, header_len);

	addr_recv.sin_family = AF_INET;
	addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_recv.sin_port = htons(RECV_PORT);

	if (bind(sock_fd, (struct sockaddr *)&addr_recv, addr_len) < 0) {
		perror("bind error.");
		exit(1);
	}

	int recv_pkt = 0;
	int recv_num;
	while (true) {
		// printf("recver wait.\n");

		recv_num = recvfrom(sock_fd, (char *)&com_header, header_len, 0,
			   	(struct sockaddr *)&addr_send, (socklen_t *)&addr_len);

		printf("receive: %d / %d bytes (PKT : %d, FG : %d)\n", recv_num, header_len,
				++recv_pkt, com_header.mih.exists_fg);

		if (recv_num < 0) {
			perror("recvfrom error.");
			exit(1);
		}

		com_header.mih.switch_id = be16toh(com_header.mih.switch_id);
		com_header.mih.timestamp = be64toh(com_header.mih.timestamp);
		com_header.sfh.switch_id = be16toh(com_header.sfh.switch_id);
		com_header.sfh.fgment_id = be32toh(com_header.sfh.fgment_id);

		for (int i = 0; i < 10; i++) {
			com_header.sfh.delay[i] = be32toh(com_header.sfh.delay[i]);
		}

		if (com_header.mih.exists_fg != 0) {
			// printf("mih.switch_id : %u\n" , com_header.mih.switch_id);
			// printf("mih.timestamp : %lu\n", com_header.mih.timestamp);
			// printf("sfh.switch_id : %u\n" , com_header.sfh.switch_id);
			// printf("sfh.sketch_fg : %u\n" , com_header.sfh.sketch_fg);
			// printf("sfh.fgment_id : %u\n" , com_header.sfh.fgment_id);

			// printf("sketch fragment :\n");

			// for (int i = 0; i < 10; i++) {
			// 	printf("sfh.delay_%d : %u\n", i, com_header.sfh.delay[i]);
			// }
			// printf("\n");

			// receive_fragment(com_header);
		}
	}

	close(sock_fd);

	return 0;
}
