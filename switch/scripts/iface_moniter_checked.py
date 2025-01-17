import nnpy
import time
import struct
from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI
from scapy.all import *
import sys
import threading
import argparse



class packetReceiver():

	def __init__(self, sw_name, iface_name, logf):
		self.sw_name = str(sw_name)
		self.iface_name = str(iface_name)
		self.topo = Topology(db="../p4src_flowsize/topology.db")
		self.logf = logf

		self.thrift_port = self.topo.get_thrift_port(sw_name)
		self.counter = 1

		logf = open(self.logf, "w")
		logf.write(self.sw_name + "-" + self.iface_name + "\n")
		logf.close()
		
	def recv_msg_cpu(self, pkt):
		self.counter += 1
		ether = pkt.getlayer(Ether)
		ip = pkt.getlayer(IP)
		tcp=pkt.getlayer(TCP)
		udp=pkt.getlayer(UDP)
		icmp=pkt.getlayer(ICMP)
		if(ip!=None and udp!=None):
			print("get\n")
			#ls(ip)
			self.gen_per_packet_log(ip,udp)
		else:
			pass
	def gen_per_packet_log(self,ip,udp):
		
		logs=open(self.logf,"a")
		change=lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
		
		# srcAddr=change(ip.src)
		# dstAddr=change(ip.dst)
		
		logs.write('{"switch name":"'+self.sw_name+'",')
		logs.write('"packet number":"'+str(self.counter-1)+'","packet_info":{')
		logs.write('"srcAddr":"'+str(ip.src)+'",')
		logs.write('"dstAddr":"'+str(ip.dst)+'",')
		logs.write('"protocol":"'+str(ip.proto)+'",')
		logs.write('"srcPort":"'+str(udp.sport)+'",')
		logs.write('"dstPort":"'+str(udp.dport)+'",')
		logs.write(" }}\n")
		logs.close()

	def run(self):
		sniff(iface=self.sw_name+"-"+self.iface_name, prn=self.recv_msg_cpu)


if __name__ == "__main__":
	controller = packetReceiver(sys.argv[1], sys.argv[2], sys.argv[3])
	controller.run()
