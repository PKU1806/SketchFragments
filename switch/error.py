# take 2 para
# 1 for error type  2 for error desc

# for loop: 
# 1. get the topology 
# 2. get a loop 
# 3. change the corresponding mac table 
# 4. record the right one? no need

# for blackhole 
# 1. shutdown a switch
# 2. drop every packet

import nnpy
import struct
from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import SimpleSwitchAPI
import sys
import argparse
import random



class GenFault(object):

    def __init__(self):
        self.topo = Topology(db="topology.db")  #set the topology
        self.controllers = {}                   #the switches
        self.init()

    def init(self):
        self.connect_to_switches()              

    def connect_to_switches(self):
        for p4switch in self.topo.get_p4switches():# topology line 632
            thrift_port = self.topo.get_thrift_port(p4switch) 
            self.controllers[p4switch] = SimpleSwitchAPI(thrift_port)

    def loop(self,number):
        pass
        #todo 

    def blackhole(self,args):
        if args.sw_name== None:
            pass
            print "Not implemented yet"
        else:
            for sw_name,controller in self.controllers.items():
                if sw_name==args.sw_name:
                    controller.table_clear("ecmp_group_to_nhop")
                    controller.table_clear("ipv4_lpm")
                    print sw_name,"has been shut down"
                    break
                    
        
        #delete all items of mac and l2map



    def route(self):
        switch_ecmp_groups = {sw_name:{} for sw_name in self.topo.get_p4switches().keys()}
        for sw_name, controllers in self.controllers.items():
            for sw_dst in self.topo.get_p4switches():
                #if its ourselves we create direct connections
                if sw_name == sw_dst:
                    for host in self.topo.get_hosts_connected_to(sw_name):
                        sw_port = self.topo.node_to_node_port_num(sw_name, host)
                        host_ip = self.topo.get_host_ip(host) + "/32"
                        host_mac = self.topo.get_host_mac(host)

                        #add rule
                        print "table_add at {}:".format(sw_name)
                        self.controllers[sw_name].table_add("ipv4_lpm", "set_nhop", [str(host_ip)], [str(host_mac), str(sw_port)])

                #check if there are directly connected hosts
                else:
                    if self.topo.get_hosts_connected_to(sw_dst):
                        paths = self.topo.get_shortest_paths_between_nodes(sw_name, sw_dst)
                        for host in self.topo.get_hosts_connected_to(sw_dst):

                            if len(paths) == 1:
                                next_hop = paths[0][1]

                                host_ip = self.topo.get_host_ip(host) + "/24"
                                sw_port = self.topo.node_to_node_port_num(sw_name, next_hop)
                                dst_sw_mac = self.topo.node_to_node_mac(next_hop, sw_name)

                                #add rule
                                print "table_add at {}:".format(sw_name)
                                self.controllers[sw_name].table_add("ipv4_lpm", "set_nhop", [str(host_ip)],
                                                                    [str(dst_sw_mac), str(sw_port)])

                            elif len(paths) > 1:
                                next_hops = [x[1] for x in paths]
                                dst_macs_ports = [(self.topo.node_to_node_mac(next_hop, sw_name),
                                                   self.topo.node_to_node_port_num(sw_name, next_hop))
                                                  for next_hop in next_hops]
                                host_ip = self.topo.get_host_ip(host) + "/24"

                                #check if the ecmp group already exists. The ecmp group is defined by the number of next
                                #ports used, thus we can use dst_macs_ports as key
                                if switch_ecmp_groups[sw_name].get(tuple(dst_macs_ports), None):
                                    ecmp_group_id = switch_ecmp_groups[sw_name].get(tuple(dst_macs_ports), None)
                                    print "table_add at {}:".format(sw_name)
                                    self.controllers[sw_name].table_add("ipv4_lpm", "ecmp_group", [str(host_ip)],
                                                                        [str(ecmp_group_id), str(len(dst_macs_ports))])

                                #new ecmp group for this switch
                                else:
                                    new_ecmp_group_id = len(switch_ecmp_groups[sw_name]) + 1
                                    switch_ecmp_groups[sw_name][tuple(dst_macs_ports)] = new_ecmp_group_id

                                    #add group
                                    for i, (mac, port) in enumerate(dst_macs_ports):
                                        print "table_add at {}:".format(sw_name)
                                        self.controllers[sw_name].table_add("ecmp_group_to_nhop", "set_nhop",
                                                                            [str(new_ecmp_group_id), str(i)],
                                                                            [str(mac), str(port)])

                                    #add forwarding rule
                                    print "table_add at {}:".format(sw_name)
                                    self.controllers[sw_name].table_add("ipv4_lpm", "ecmp_group", [str(host_ip)],
                                                                        [str(new_ecmp_group_id), str(len(dst_macs_ports))])
		

    

if __name__ == "__main__":
    parser=argparse.ArgumentParser()
    parser.add_argument("type",help="the type wanted for the net",choices=["loop","blackhole","reset"],default="reset")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-n","--number",help="the loop's length or the number for blackhole",type=int,default=1)
    group.add_argument("-s","--sw_name",help="specify the blackhole host")

    args=parser.parse_args()
    fault=GenFault()
    if args.type=="loop":
        fault.loop(args.number)
    elif args.type=="blackhole":
        fault.blackhole(args)
    else:
        fault.route()
