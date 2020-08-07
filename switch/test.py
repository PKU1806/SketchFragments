import nnpy
import struct
from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI
from scapy.all import *
import sys

import argparse


class CPU(Packet):
    name="CPU"
    fields_desc=[\
            BitField('srcAddr',0,32),\
            BitField('dstAddr',0,32),\
            BitField('protocol',0,8),\
            BitField('srcPort',0,16),\
            BitField('dstPort',0,16),\
            BitField('delay',0,48),\
            BitField('interval',0,48)]


class packetReceicer(object):

    def __init__(self, sw_name):
        self.topo = Topology(db="topology.db")
        self.sw_name = sw_name
        self.thrift_port = self.topo.get_thrift_port(sw_name)
        self.cpu_port =  self.topo.get_cpu_port_index(self.sw_name)
        self.controller = SimpleSwitchAPI(self.thrift_port)
        self.flow={}
        self.init()

    def init(self):
        self.add_mirror()
        self.counter=1
        self.logs=open("./switch_log/"+self.sw_name+".log","w")
        self.logs_info=open("./switch_log/"+self.sw_name+"info.log","w")
        logs.close()
        logs_info.close()

    def add_mirror(self):
        if self.cpu_port:
            self.controller.mirroring_add(100, self.cpu_port) # correspond to the 100 in p4 code
            #is there any probability to increase the mirro port to add cpu port?

    def recv_msg_cpu(self, pkt):
        print
        print("received packet number:"+str(self.counter))
        self.counter+=1
        cpu=CPU(str(pkt))
        ls(cpu)
        self.gen_per_packet_log(cpu)

    
    def gen_log(self,cpu):
        logs_info=open("./switch_log/"+self.sw_name+"info.log","a")
        flow_key=str(cpu.srcAddr)+str(cpu.dstAddr)+str(cpu.protocol)+str(cpu.srcPort)+str(spu.dstPort)
        if self.flow.has_key(flow_key):
            self.flow[flow_key]+=1
        else:
            self.flow[flow_key]=1

    def gen_per_packet_log(self,cpu):
        logs=open("./switch_log/"+self.sw_name+".log","a")
        change=lambda x: '.'.join([str(x/(256**i)%256) for i in range(3,-1,-1)])
        
        srcAddr=change(cpu.srcAddr)
        dstAddr=change(cpu.dstAddr)
        tmp_delay=str(cpu.delay)
        delay=tmp_delay[-12:-9]+"s "+tmp_delay[-9:-6]+"ms "+tmp_delay[-6:-3]+"us "+tmp_delay[-3:]+"ns"
        
        logs.write("SWITCH["+self.sw_name+"] " )
        logs.write("[Packet No."+str(self.counter-1)+"] {")
        logs.write(" [srcAddr : "+str(srcAddr)+"]")
        logs.write(" [dsrAddr : "+str(dstAddr)+"]")
        logs.write(" [protocol : "+str(cpu.protocol)+"]")
        logs.write(" [srcPort :"+str(cpu.srcPort)+"]")
        logs.write(" [dstPort :"+str(cpu.dstPort)+"]")
        logs.write(" [delay :"+delay+"]")
        logs.write(" }\n")
        logs.close()


    

    def run_cpu_port_loop(self):
        
        cpu_port_intf = str(self.topo.get_cpu_port_intf(self.sw_name).replace("eth0", "eth1"))
        # the cpu has two ports   could use two thread to sniff
        print(sniff(iface=cpu_port_intf, prn=self.recv_msg_cpu))


if __name__ == "__main__":
    #usage : python test.py [sw_name]
    #hint: the sw_name must open the cpu_port function in p4app.json
    #"cpu_port" : true

    parser=argparse.ArgumentParser()
    
    parser.add_argument("switch",help="this switch's name")
    args=parser.parse_args()
    sw_name = args.switch
    controller = packetReceicer(sw_name).run_cpu_port_loop()
