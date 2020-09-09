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
		self.counter = 0

		logf = open(self.logf, "w")
		logf.write(self.sw_name + "-" + self.iface_name + "\n")
		logf.close()
		
	def recv_msg_cpu(self, pkt):
		self.counter += 1
		logf = open(self.logf, "a")
		logf.write(self.sw_name + "-" + self.iface_name + " : " + str(self.counter) + "\n")
		logf.close()

	def run(self):
		print (sniff(iface=self.sw_name+"-"+self.iface_name, prn=self.recv_msg_cpu))


if __name__ == "__main__":
	controller = packetReceiver(sys.argv[1], sys.argv[2], sys.argv[3])
	controller.run()
