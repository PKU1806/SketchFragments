#!/usr/bin/env python
import sys
import socket
import random
import time
from subprocess import Popen, PIPE
import re
from threading import Thread, Event
from scapy.all import *



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
def send_packet(interface):
    
    print(sys.argv[1])
    dstAddr = socket.gethostbyname("10.8.4.2")
    print(dstAddr)
    
    #print(socket.getaddrinfo(sys.argv[1], None, 0, socket.SOCK_STREAM))
    '''
    ether_dst = get_dst_mac(dstAddr)

    if not ether_dst:
        print "Mac address for %s was not found in the ARP table" % dstAddr
        exit(1)
    
    pkt= Ether(src=get_if_hwaddr(interface),dst=ether_dst)
    pkt=pkt/IP(dst=dstAddr)
    '''

    pkt=IP(dst=dstAddr)
    pkt=pkt/TCP()
    while True:
        raw_input("Testing! Press the return key to send a packet")
        print "Sending on interface %s \n"%(interface)
        #sendp(pkt, iface=iface, verbose=False)
        send(pkt)



def main():

    if len(sys.argv)<2:
        print 'pass 1 argument: <destination sw_name>'
        exit(1)

    interface=get_if()
    send_packet(interface)


if __name__ == "__main__":
    main()