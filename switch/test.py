import nnpy
import struct
from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI
from scapy.all import *

class CPU(Packet):
    name="CPU"
    fields_desc=[\
            BitField('srcAddr',0,32),\
            BitField('dstAddr',0,32),\
            BitField('protocol',0,8),\
            BitField('srcPort',0,16),\
            BitField('dstPort',0,16),\
            BitField('delay',0,48)]




class packetReceicer(object):

    def __init__(self, sw_name):
        self.topo = Topology(db="topology.db")
        self.sw_name = sw_name
        self.thrift_port = self.topo.get_thrift_port(sw_name)
        self.cpu_port =  self.topo.get_cpu_port_index(self.sw_name)
        self.controller = SimpleSwitchAPI(self.thrift_port)
        self.init()

    def init(self):
        self.add_mirror()
        self.counter=0

    def add_mirror(self):
        if self.cpu_port:
            self.controller.mirroring_add(100, self.cpu_port) # correspond to the 100 in p4 code
            #??????????? is there any probability to increase the mirro port to add cpu port?

    def recv_msg_cpu(self, pkt):
        print
        print("received packet number:"+str(self.counter))
        self.counter+=1
        packet = Ether(str(pkt))
        ls(packet)
        print("Ether payload :")
        print(packet.payload)
        #need further modification
        
        if packet.type == 0x1234:
            cpu = CPU(packet.payload)
            print("cpu info:")
            ls(cpu)
            


    def run_cpu_port_loop(self):
        
        cpu_port_intf = str(self.topo.get_cpu_port_intf(self.sw_name).replace("eth0", "eth1"))
        #need further modification 
        # the cpu has two ports  maybe could use two thread to sniff
        print(sniff(iface=cpu_port_intf, prn=self.recv_msg_cpu))


if __name__ == "__main__":
    #usage : python test.py [sw_name]
    #hint: the sw_name must open the cpu_port function in p4app.json
    #"cpu_port" : true

    import sys
    sw_name = sys.argv[1]
    controller = packetReceicer(sw_name).run_cpu_port_loop()
