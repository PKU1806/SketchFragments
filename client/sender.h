#define MAX_IP_LENGTH 128

struct Sender {
	char recv_ip[MAX_IP_LENGTH];

	sockaddr_in addr_recv;
	MIH_Header mih_header;

	int recv_port, sock_fd, addr_len, header_len;

	Sender(char *ip, int port) {
		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock_fd < 0) {
			perror("socket error.");
			exit(1);
		}

		strcpy(recv_ip, ip);
		recv_port = port;

		printf("ip: %s, port: %d\n", recv_ip, recv_port);

		addr_len = sizeof(addr_recv);
		header_len = sizeof(mih_header);

		memset((char *)&addr_recv, 0x00, addr_len);
		memset((char *)&mih_header, 0x00, header_len);

		addr_recv.sin_family = AF_INET;
		addr_recv.sin_addr.s_addr = inet_addr(recv_ip);
		addr_recv.sin_port = htons(recv_port);

		mih_header.exists_fg = 0;
	}

	void send(int max_pkt) {
		for (int send_pkt = 0; send_pkt < max_pkt; send_pkt++) {
			int send_num = sendto(sock_fd, (char *)&mih_header, header_len,
				   	0, (sockaddr *)&addr_recv, addr_len);

			printf("send: %d / %d bytes (PKT : %d)\n", send_num, header_len, send_pkt);

			if (send_num < 0) {
				perror("sendto error.");
				exit(1);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}

	~Sender() {
		close(sock_fd);
	}
};
