#include <map>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include <sched.h>

#include "sender.h"

using namespace std;

#define MAX_PATH 1024

map<string, string> sender_pid, receiver_ip;

void load_sender_config(const char * path) {
	ifstream sender_config(path, std::ios::in);

	string sender, pid;
	while (sender_config >> sender >> pid) {
		sender_pid[sender] = pid;
	}
}

void load_receiver_config(const char * path) {
	ifstream receiver_config(path, std::ios::in);

	string receiver, ip;
	while (receiver_config >> receiver >> ip) {
		receiver_ip[receiver] = ip;
	}
}

struct Thread_Sender {
	Sender *sender;

	Thread_Sender(string send_pid, string recv_ip, int recv_port) {
		switch_namespace(send_pid);
		sender = new Sender(recv_ip, recv_port);
	}

	void send(int max_pkt) {
		sender->send(max_pkt);
	}

	void switch_namespace(string pid) {
		char path[MAX_PATH];

		sprintf(path, "/proc/%s/ns/net", pid.c_str());
		std::cout << path << std::endl;
		printf("attach net: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/pid", pid.c_str());
		std::cout << path << std::endl;
		printf("attach pid: %d.\n", attachToNS(path));

		sprintf(path, "/proc/%s/ns/mnt", pid.c_str());
		std::cout << path << std::endl;
		printf("attach mnt: %d.\n", attachToNS(path));
	}

	int attachToNS(char *path) {
		int nsid = open(path, O_RDONLY);
		return setns(nsid, 0);
	}

	~Thread_Sender() {
		delete sender;
	}
};

void Thread_Sender_Function(string sender_name, string receiver_name,
	   	int recv_port, int max_pkt) {
	Thread_Sender sender(sender_pid[sender_name], 
			receiver_ip[receiver_name], recv_port);
	sender.send(max_pkt);

	std::cout << sender_name << " => " << receiver_name << ":" << 
		recv_port << " : " << max_pkt << std::endl;
}

int main(int argc, char **argv) {
	system("./config.sh");

	load_sender_config("./sender.config");
	load_receiver_config("./receiver.config");

	std::cout << "sender host:" << std:: endl;
	for (auto i: sender_pid) {
		std::cout << i.first << " : " << i.second << std::endl;
	}

	std::cout << "receiver host:" << std:: endl;
	for (auto i: receiver_ip) {
		std::cout << i.first << " : " << i.second << std::endl;
	}

	std::thread sender_3_1(Thread_Sender_Function, "h3", "h1", 8000, 100);
	std::thread sender_4_1(Thread_Sender_Function, "h4", "h1", 8000, 100);
	std::thread sender_5_1(Thread_Sender_Function, "h5", "h1", 8000, 100);
	std::thread sender_6_1(Thread_Sender_Function, "h6", "h1", 8000, 100);
	std::thread sender_7_1(Thread_Sender_Function, "h7", "h1", 8000, 100);
	std::thread sender_8_1(Thread_Sender_Function, "h8", "h1", 8000, 100);


	std::thread sender_3_2(Thread_Sender_Function, "h3", "h2", 8000, 100);
	std::thread sender_4_2(Thread_Sender_Function, "h4", "h2", 8000, 100);
	std::thread sender_5_2(Thread_Sender_Function, "h5", "h2", 8000, 100);
	std::thread sender_6_2(Thread_Sender_Function, "h6", "h2", 8000, 100);
	std::thread sender_7_2(Thread_Sender_Function, "h7", "h2", 8000, 100);
	std::thread sender_8_2(Thread_Sender_Function, "h8", "h2", 8000, 100);

	sender_3_1.join();
	sender_4_1.join();
	sender_5_1.join();
	sender_6_1.join();
	sender_7_1.join();
	sender_8_1.join();

	sender_3_2.join();
	sender_4_2.join();
	sender_5_2.join();
	sender_6_2.join();
	sender_7_2.join();
	sender_8_2.join();

	return 0;
}
