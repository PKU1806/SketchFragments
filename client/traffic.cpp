#include <map>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include <sched.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "header.h"
#include "sender.h"
#include "receiver.h"
#include "aggregator.h"

using namespace std;

const int udp_port = 1806;

string hname[MAX_HOST_NUM];
map<string, string> host_pid, host_ip;

void Connector_S(string sname, string rname, int rport, int maxp) {
	Simulator::Sender sender(host_ip[rname], rport, host_pid[sname]);

	sender.send(maxp);
}

void Connector_R(string rname, int rport, int maxp, 
		Simulator::Aggregator *agg) {
	Simulator::Receiver receiver(rport, agg, host_pid[rname]);

	receiver.receive(maxp);
}

int config_loader() {
	if (system("./config.sh") != 0) {
		exit(0);
	}

	ifstream loader("./host.config", std::ios::in);

	int host_num = 0;
	string name, pid, ip;

	while (loader >> hname[host_num] >> pid >> ip) {
		host_pid[hname[host_num]] = pid;
		host_ip[hname[host_num]] = ip;
		host_num += 1;
	}

	std::cout << "hosts:" << std:: endl;
	for (int i = 0; i < host_num; i++) {
		std::cout << hname[i] << " : (" << host_pid[hname[i]] << ", " <<
		   	host_ip[hname[i]] << ")" << std::endl;
	}

	return host_num;
}

void traffic_generator(int host_num, int conn_num, int maxp, int maxi) {
	Simulator::Aggregator aggregator(Simulator::sw_num, Simulator::ar_num, 
			Simulator::bu_num);

	std::thread **receiver = new std::thread*[host_num];

	for (int i = 0; i < host_num; i++) {
		receiver[i] = new std::thread(Connector_R, hname[i], udp_port, -1,
			   	&aggregator);
	}

	std::thread **sender = new std::thread*[conn_num];

	for (int i = 0; i < conn_num; i++) {
		int sendi = rand() % host_num;
		int recvi = rand() % host_num;
		int sendp = rand() % maxp + 1;
		int iterv = rand() % maxi + 1;

		string sname = hname[sendi];
		string rname = hname[recvi];

		sender[i] = new std::thread(Connector_S, sname, rname, udp_port, sendp);

		std::cout << sname << " => " << rname << ":" << udp_port << "(" << 
			sendp << ")" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(iterv));
	}

	for (int i = 0; i < host_num; i++) {
		receiver[i]->join();
	}
	
	for (int i = 0; i < conn_num; i++) {
		sender[i]->join();
	}
}

int main(int argc, char **argv) {
	if (argc != 4) {
		perror("args error.");
		exit(1);
	}

	int host_num = config_loader();
	int conn_num = atoi(argv[1]);

	int maxp = atoi(argv[2]);
	int maxi = atoi(argv[3]);

	traffic_generator(host_num, conn_num, maxp, maxi);

	return 0;
}
