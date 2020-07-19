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


const int SWITCH_NUM = 6;
const int ARRAY_NUM = 3;
const int BUCKET_NUM = 16;

SF_Header sketch[SWITCH_NUM][ARRAY_NUM][BUCKET_NUM];

int sketch_flag[SWITCH_NUM], sketch_fnum[SWITCH_NUM];

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
		// printf("recver wait.\n");

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

		for (int i = 0; i < 10; i++) {
			sf_header.sfh_delay[i] = be32toh(sf_header.sfh_delay[i]);
		}

		if (sf_header.sfh_switch_id != 0) {
			// printf("mih_switch_id : %u\n" , sf_header.mih_switch_id);
			// printf("mih_timestamp : %lu\n", sf_header.mih_timestamp);
			// printf("sfh_switch_id : %u\n" , sf_header.sfh_switch_id);
			// printf("sfh_sketch_fg : %u\n" , sf_header.sfh_sketch_fg);
			// printf("sfh_fgment_id : %u\n" , sf_header.sfh_fgment_id);

			// printf("sketch fragment :\n");

			// for (int i = 0; i < 10; i++) {
			// 	printf("sfh_delay_%d : %u\n", i, sf_header.sfh_delay[i]);
			// }
			// printf("\n");

			int switch_id = sf_header.sfh_switch_id - 8001;
			int sketch_fg = sf_header.sfh_sketch_fg;
			int array_id = sf_header.sfh_fgment_id / BUCKET_NUM;
			int bucket_id = sf_header.sfh_fgment_id % BUCKET_NUM;

			if (sketch_flag[switch_id] != sketch_fg) {
				printf("switch %d detect sketch %d.\n", switch_id + 1, sketch_fg);

				sketch_flag[switch_id] = sketch_fg;
				sketch_fnum[switch_id] = 0;

				memset(sketch[switch_id], 0x00, ARRAY_NUM * BUCKET_NUM * sizeof(SF_Header));
			}

			if (sketch[switch_id][array_id][bucket_id].sfh_switch_id == 0) {
				printf("switch %d detect new fragment %d.\n", switch_id + 1, sf_header.sfh_fgment_id);

				sketch[switch_id][array_id][bucket_id] = sf_header;
				sketch_fnum[switch_id] += 1;

				if (sketch_fnum[switch_id] == BUCKET_NUM * ARRAY_NUM) {
					printf("switch %d recieve sketch %d.\n", switch_id + 1, sketch_fg);

					for (int i = 0; i < ARRAY_NUM; i++) {
						printf("sketch array %d.\n", i);

						for (int j = 0; j < 10; j++) {
							for (int k = 0; k < BUCKET_NUM; k++) {
								printf("|%3u|", sketch[switch_id][i][k].sfh_delay[j]);
							}
							printf("\n");
						}
					}
				}
			}
		}
	}

	close(sock_fd);

	return 0;
}
