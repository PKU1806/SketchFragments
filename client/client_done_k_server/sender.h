#ifndef __SENDER_H__
#define __SENDER_H__

#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/file.h>

#include "header.h"
#include "host.h" 

namespace Simulator {

struct Sender : Host {
	std::string recv_ip;

	sockaddr_in addr_recv;
	SEND_Header send_header;

	int recv_port, sock_fd, addr_len, header_len;

	Sender(std::string ip, int port, std::string pid) : Host(pid) {

		sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

		if (sock_fd < 0) {
			perror("socket error.");
			exit(1);
		}

		recv_ip = ip;
		recv_port = port;

		// printf("ip: %s, port: %d\n", recv_ip.c_str(), recv_port);

		addr_len = sizeof(addr_recv);
		header_len = sizeof(send_header);

		memset((char *)&addr_recv, 0x00, addr_len);
		memset((char *)&send_header.load_header, 0xFF, sizeof(send_header.load_header));

		addr_recv.sin_family = AF_INET;
		addr_recv.sin_addr.s_addr = inet_addr(recv_ip.c_str());
		addr_recv.sin_port = htons(recv_port);

		send_header.flag_header.exists_fg = 0; 
	}

	void send(int maxp) {
		for (int sendp = 0; sendp < maxp || maxp < 0; sendp++) {
			int send_num = sendto(sock_fd, (char *)&send_header, header_len,
				   	0, (sockaddr *)&addr_recv, addr_len);

			// printf("send: %d / %d bytes (PKT : %d)\n", send_num, header_len, sendp);
			// printf("recv ip: %s, recv port: %d\n", recv_ip.c_str(), recv_port);

			if (send_num < 0) {
				perror("sendto error.");
				exit(1);
			}

			int fd = open("log.txt", O_WRONLY | O_APPEND);
			if (fd == -1) {
				perror("open file error!");
				exit(1);
			}
			
			flock(fd, LOCK_EX);

			int pkt_num = (++Simulator::actual_packet_num);
			char text[] = "It has sent packet: ";
			char DataBuffer[20];
			char temp[] = "\n";
			sprintf(DataBuffer, "%d", pkt_num);
			int wr_text = write(fd, text, strlen(text));
			int wr_DataBuffer = write(fd, DataBuffer, strlen(DataBuffer));
			int wr_temp = write(fd, temp, strlen(temp));

			if (wr_text == -1 || wr_DataBuffer == -1 || wr_temp == -1)
			{
				perror("print log.txt error !");
				exit(1);
			}
			
			flock(fd, LOCK_UN);
			close(fd);
			
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	~Sender() {
		close(sock_fd);
	}
};
}

#endif
