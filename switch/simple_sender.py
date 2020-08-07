#!/usr/bin/env python
import sys
import socket
import random
import time
import argparse
from subprocess import Popen, PIPE
import re
from threading import Thread, Event
from scapy.all import *
from p4utils.utils.topology import Topology

topo = Topology(db="topology.db")

#always via eth0
def get_if():
    ifs=get_if_list()
    iface=None # "h1-eth0"
    for i in get_if_list():
        if "eth0" in i:
            iface=i
            break;
    if not iface:
        print "Cannot find eth0 interface"
        exit(1)
    return iface




def get_dst_mac(ip):

    try:
        pid = Popen(["arp", "-n", ip], stdout=PIPE)
        s = pid.communicate()[0]
        mac = re.search(r"(([a-f\d]{1,2}\:){5}[a-f\d]{1,2})", s).groups()[0]
        return mac
    except:
        return None

#whether l3 send can be use?
#and how to set?
def send_packet(interface,args):
    
    dstAddr=topo.get_host_ip(args.d)
    #dstAddr = socket.gethostbyname(args.d)
    
    #print(socket.getaddrinfo(sys.argv[1], None, 0, socket.SOCK_STREAM))
    #ether_dst = get_dst_mac(dstAddr)
    #if not ether_dst:
    #    print "Mac address for %s was not found in the ARP table" % dstAddr
    #    exit(1)
    #pkt= Ether(src=get_if_hwaddr(interface),dst=ether_dst)
    #pkt=pkt/IP(dst=dstAddr)
    

    #if want to send TCP ,must change the parser of p4
    pkt=IP(dst=dstAddr)
    
    if args.type=="tcp" or args.type=="TCP":
        pkt=pkt/TCP()
    else:
        pkt=pkt/UDP()
    pkt=pkt/"loadloadloadloadloadloadload"
    while True:
        raw_input("Testing! Press the return key to send a packet using "+args.type.lower())
        print "Sending on interface %s \n"%(interface)
        #sendp(pkt, iface=iface, verbose=False)
        for i in range(args.number):
            send(pkt)



def main():
    parser=argparse.ArgumentParser()
    
    #parser.add_argument("d",help="the dst IP addr of host")
    parser.add_argument("d",help="the dst host name")
    parser.add_argument("-t","--type",help="the packet type to be sent",default="udp",choices=["udp","tcp"])
    parser.add_argument("-n","--number",help="the packet number to be sent",type=int,default=1)
    args=parser.parse_args()
    

    interface=get_if()
    send_packet(interface,args)


if __name__ == "__main__":
    main()