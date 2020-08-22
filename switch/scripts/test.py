import nnpy
import struct
from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI
import sys
import argparse
import random

class GenFault(object):
    def __init__(self):
        self.topo = Topology(db="../p4src_flowsize/topology.db")  #set the topology
        self.controllers = {}                   #the switches
        self.init()

    def init(self):
        self.connect_to_switches()              

    def connect_to_switches(self):
        for p4switch in self.topo.get_p4switches():
            thrift_port = self.topo.get_thrift_port(p4switch) 
            self.controllers[p4switch] = SimpleSwitchAPI(thrift_port)

    def loop(self):
        switches=raw_input("type the switch's name to gen loop,seperated by ','\nmust be physically loop-able:\n").split(',')
        
        IPs=[]
        for sw_name in self.controllers.keys():
            for host in self.topo.get_hosts_connected_to(sw_name):
                host_ip = self.topo.get_host_ip(host) + "/24"
                IPs.append(host_ip)
        
        for i in range(len(switches)):
            sw_name=switches[i]
            self.controllers[sw_name].table_clear("ecmp_group_to_nhop")
            self.controllers[sw_name].table_clear("ipv4_lpm")
            
            #next_hop=NULL
            if i==len(switches)-1:
                next_hop=switches[0]
            else:
                next_hop=switches[i+1]

            sw_port = self.topo.node_to_node_port_num(sw_name, next_hop)
            dst_sw_mac = self.topo.node_to_node_mac(next_hop, sw_name)
            #print "table_add at {}:".format(sw_name)
            for host_ip in IPs:
                self.controllers[sw_name].table_add("ipv4_lpm", "set_nhop", [str(host_ip)],\
                                [str(dst_sw_mac), str(sw_port)])


    def blackhole(self,args):
        if args.sw_name== None:
            pass
            print "Not implemented yet,please specify the switch name"
        else:
            self.controllers[args.sw_name].table_clear("ecmp_group_to_nhop")
            self.controllers[args.sw_name].table_clear("ipv4_lpm")
            print args.sw_name,"has been shut down"

    def test(self):
        

    

if __name__ == "__main__":
    fault=GenFault().test()

