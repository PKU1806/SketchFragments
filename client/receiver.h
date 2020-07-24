#define MAX_IP_LENGTH 1024

#define SWITCH_NUM 10
#define ARRAY_NUM 3
#define BUCKET_NUM 64

namespace Receiver {

using namespace std::chrono;

struct Receiver {
	SFH_Header sketch[SWITCH_NUM][ARRAY_NUM][BUCKET_NUM];

	int sketch_flag[SWITCH_NUM], sketch_fnum[SWITCH_NUM], 
		packet_start[SWITCH_NUM], packet_epoch[SWITCH_NUM];

	steady_clock::time_point time_start[SWITCH_NUM];
	milliseconds time_epoch[SWITCH_NUM];

	sockaddr_in addr_send, addr_recv;
	COM_Header com_header;

	int sock_fd, recv_port, addr_len, header_len;

	Receiver(int port) {
		memset(sketch_flag, 0xFF, sizeof(sketch_flag));
		memset(sketch_fnum, 0x00, sizeof(sketch_fnum));

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

	void receive(int max_pkt) {
		for (int recv_pkt = 0; recv_pkt < max_pkt || max_pkt < 0; recv_pkt++) {
			int recv_num = recvfrom(sock_fd, (char *)&com_header, header_len,
				   	0, (sockaddr *)&addr_send, (socklen_t *)&addr_len);

			// printf("receive: %d / %d bytes (PKT : %d, FG : %d)\n", recv_num,
			// 	   	header_len, recv_pkt, com_header.mih.exists_fg);

			if (recv_num < 0) {
				perror("recvfrom error.");
				exit(1);
			}

			betoh_header(com_header);
			if (com_header.mih.exists_fg != 0) {
				receive_header(com_header);
			}
		}
	}

	void betoh_header(COM_Header &com_header) {
		com_header.mih.switch_id = be16toh(com_header.mih.switch_id);
		com_header.mih.timestamp = be64toh(com_header.mih.timestamp);
		com_header.sfh.switch_id = be16toh(com_header.sfh.switch_id);
		com_header.sfh.fgment_id = be32toh(com_header.sfh.fgment_id);

		for (int i = 0; i < 10; i++) {
			com_header.sfh.delay[i] = be32toh(com_header.sfh.delay[i]);
		}
	}

	void display_header(COM_Header &com_header) {
		printf("mih.switch_id : %u\n" , com_header.mih.switch_id);
		printf("mih.timestamp : %lu\n", com_header.mih.timestamp);
		printf("sfh.switch_id : %u\n" , com_header.sfh.switch_id);
		printf("sfh.sketch_fg : %u\n" , com_header.sfh.sketch_fg);
		printf("sfh.fgment_id : %u\n" , com_header.sfh.fgment_id);

		printf("sketch fragment :\n");

		for (int i = 0; i < 10; i++) {
			printf("sfh.delay_%d : %u\n", i, com_header.sfh.delay[i]);
		}
		printf("\n");
	}

	void receive_header(COM_Header &com_header) {
		int switch_id = com_header.sfh.switch_id - 8001;
		int sketch_fg = com_header.sfh.sketch_fg;
		int array_id = com_header.sfh.fgment_id / BUCKET_NUM;
		int bucket_id = com_header.sfh.fgment_id % BUCKET_NUM;

		if (sketch_flag[switch_id] != sketch_fg) {
			receive_next_sketch(switch_id, sketch_fg);
		}

		// printf("switch id: %d, array id: %d, bucket id: %d.\n",
		// 		switch_id, array_id, bucket_id);

		if (sketch[switch_id][array_id][bucket_id].switch_id == 0) {
			receive_next_fgment(com_header.sfh, 
					switch_id, sketch_fg, array_id, bucket_id);
		}
	}

	void receive_next_sketch(int switch_id, int sketch_fg) {
		printf("switch %d detect sketch %d.\n", switch_id + 1, sketch_fg);

		sketch_flag[switch_id] = sketch_fg;
		sketch_fnum[switch_id] = 0;

		memset(sketch[switch_id], 0x00, ARRAY_NUM * BUCKET_NUM * 
				sizeof(SFH_Header));

		time_start[switch_id] = steady_clock::now();
	}

	void receive_next_fgment(SFH_Header &sfh_header,
			int switch_id, int sketch_fg, int array_id, int bucket_id) {
		printf("switch %d detect new fragment %d.\n", switch_id + 1,
			   	sfh_header.fgment_id);

		sketch[switch_id][array_id][bucket_id] = sfh_header;
		sketch_fnum[switch_id] += 1;

		if (sketch_fnum[switch_id] == BUCKET_NUM * ARRAY_NUM) {
			receive_full_sketch(switch_id, sketch_fg);
		}
	}

	void receive_full_sketch(int switch_id, int sketch_fg) {
		auto time_end = steady_clock::now();
		time_epoch[switch_id] = duration_cast<milliseconds>
			(time_end - time_start[switch_id]);
		visor_sketch(switch_id, sketch_fg, time_epoch[switch_id]);
	}

	void visor_sketch(int switch_id, int sketch_fg, milliseconds time_epoch) {
		std::cout << "switch " << switch_id + 1 << 
			" receive sketch " << sketch_fg << 
			" using " << time_epoch.count() << " milliseconds." << std::endl;

		// for (int i = 0; i < ARRAY_NUM; i++) {
		// 	printf("sketch array %d.\n", i);

		// 	for (int j = 0; j < 10; j++) {
		// 		for (int k = 0; k < BUCKET_NUM; k++) {
		// 			printf("|%3u|", sketch[switch_id][i][k].delay[j]);
		// 		}
		// 		printf("\n");
		// 	}
		// }
	}


	~Receiver() {
		close(sock_fd);
	}
};
}

