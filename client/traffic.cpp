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

#include "sender.h"
#include "receiver.h"
#include "aggregator.h"

using namespace std;

map<string, string> host_pid, host_ip;

void load_sender_config(const char * path) {
	ifstream sender_config(path, std::ios::in);

	string sender, pid;
	while (sender_config >> sender >> pid) {
		host_pid[sender] = pid;
	}
}

void load_receiver_config(const char * path) {
	ifstream receiver_config(path, std::ios::in);

	string receiver, ip;
	while (receiver_config >> receiver >> ip) {
		host_ip[receiver] = ip;
	}
}

void Connector_S(string sname, string rname, int rport, int maxp) {
	Simulator::Sender sender(host_ip[rname], rport);

	sender.switch_namespace(host_pid[sname]);
	sender.send(maxp);

	std::cout << sname << " => " << rname << ":" << rport << " : " <<
	   	maxp << std::endl;
}

void Connector_R(string rname, int rport, int maxp, 
		Simulator::Aggregator *agg) {
	Simulator::Receiver receiver(rport, *agg);

	receiver.switch_namespace(host_pid[rname]);
	receiver.receive(maxp);

	std::cout << " => " << rname << ":" << rport << " : " << maxp <<
	   	std::endl;
}

const int sw_num = 10;
const int ar_num = 3;
const int bu_num = 16;

int main(int argc, char **argv) {
	system("./config.sh");

	load_sender_config("./sender.config");
	load_receiver_config("./receiver.config");

	std::cout << "sender host:" << std:: endl;
	for (auto i: host_pid) {
		std::cout << i.first << " : " << i.second << std::endl;
	}

	std::cout << "receiver host:" << std:: endl;
	for (auto i: host_ip) {
		std::cout << i.first << " : " << i.second << std::endl;
	}

	Simulator::Aggregator aggregator(sw_num, ar_num, bu_num);

	

	// std::thread receiver_1(Connector_R, "h1", 8000, 200, &aggregator);
	// std::thread receiver_2(Connector_R, "h2", 8000, 200, &aggregator);

	// std::thread sender_3_1(Connector_S, "h3", "h1", 8000, 100);
	// std::thread sender_4_1(Connector_S, "h4", "h1", 8000, 100);
	// std::thread sender_5_1(Connector_S, "h5", "h1", 8000, 100);
	// std::thread sender_6_1(Connector_S, "h6", "h1", 8000, 100);
	// std::thread sender_7_1(Connector_S, "h7", "h1", 8000, 100);
	// std::thread sender_8_1(Connector_S, "h8", "h1", 8000, 100);


	// std::thread sender_3_2(Connector_S, "h3", "h2", 8000, 100);
	// std::thread sender_4_2(Connector_S, "h4", "h2", 8000, 100);
	// std::thread sender_5_2(Connector_S, "h5", "h2", 8000, 100);
	// std::thread sender_6_2(Connector_S, "h6", "h2", 8000, 100);
	// std::thread sender_7_2(Connector_S, "h7", "h2", 8000, 100);
	// std::thread sender_8_2(Connector_S, "h8", "h2", 8000, 100);

	// receiver_1.join();
	// receiver_2.join();

	// sender_3_1.join();
	// sender_4_1.join();
	// sender_5_1.join();
	// sender_6_1.join();
	// sender_7_1.join();
	// sender_8_1.join();

	// sender_3_2.join();
	// sender_4_2.join();
	// sender_5_2.join();
	// sender_6_2.join();
	// sender_7_2.join();
	// sender_8_2.join();

	Simulator::Sender sender("10.7.1.2", 8000);

	sender.switch_namespace(host_pid["h2"]);
	sender.send(10);

	return 0;
}
