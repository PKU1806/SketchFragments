from p4utils.utils.topology import Topology
from p4utils.utils.sswitch_API import *
import socket, struct, pickle, os
from scapy.all import Ether, sniff, Packet, BitField
import time
import sys

class Monitor(object):

    def __init__(self, sw_name):

        self.topo = Topology(db="topology.db")
        self.sw_name = sw_name
        self.thrift_port = self.topo.get_thrift_port(sw_name)
        self.controller = SimpleSwitchAPI(self.thrift_port)



    def sketch_swap(self):

        self.controller.register_write("swap_control",0,1)
    
        present_sketch=self.controller.register_read("sketch_flag",0)
        
        if present_array ==0 :
            for index in range(0,100000):
                temp=self.controller.register_read("array0",index)
                self.controller.register_write("array3",index,temp)
                temp=self.controller.register_read("array1",index)
                self.controller.register_write("array4",index,temp)
                temp=self.controller.register_read("array2",index)
                self.controller.register_write("array5",index,temp)
        else :
            for index in range(0,100000):
                temp=self.controller.register_read("array3",index)
                self.controller.register_write("array0",index,temp)
                temp=self.controller.register_read("array4",index)
                self.controller.register_write("array1",index,temp)
                temp=self.controller.register_read("array5",index)
                self.controller.register_write("array2",index,temp)
        self.controller.register_write("swap_control",0,0)
        self.controller.register_write("sketch_flag",0,1-present_sketch)
        time.sleep(58)
    

    def monitor(self):
        while True:
            current_time=self.controller.sswitch_client.get_time_elapsed_us()%60000000
            if current_time < 2000:
                print(current_time)
                #print("ssssssssssssssssssssssssssssssssss")
                self.sketch_swap()

if __name__ == "__main__":
    Monitor(sys.argv[1]).monitor()
