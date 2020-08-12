/*************************************************************************
*********************** P A R S E R  *******************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType){
            TYPE_IPV4: parse_ipv4;
            default: accept;
        }
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
		transition select(hdr.ipv4.protocol) {
			TYPE_TCP : parse_tcp;
			TYPE_UDP : parse_udp;
            TYPE_ICMP: accept;
			default : accept;
		}
    }

    state parse_tcp {
        packet.extract(hdr.tcp);

		meta.ipv4_srcPort = hdr.tcp.srcPort;
		meta.ipv4_dstPort = hdr.tcp.dstPort;

        transition select(hdr.tcp.MIH_fg){
            0 : accept;
            1 : parse_MIH_of_TCP;
            default : accept;
        }
    }

    state parse_udp {
        packet.extract(hdr.udp);

		meta.ipv4_srcPort = hdr.udp.srcPort;
		meta.ipv4_dstPort = hdr.udp.dstPort;

        transition parse_flag;
    }

    //udp only
    state parse_flag{
        packet.extract(hdr.flag);
        transition select(hdr.flag.flag){
           4: parse_MIH_of_UDP;
           6: parse_MIH_of_UDP;
           7: parse_MIH_of_UDP;
           3: parse_SFH;
           2: parse_SFH;
           default : accept;
        }
    }

    state parse_MIH_of_TCP{
		packet.extract(hdr.MIH);
        
        transition select(hdr.tcp.SFH_fg) {
			0 : accept;
			1 : parse_SFH;
			default : accept;
		}
		
	}

    state parse_MIH_of_UDP{
        packet.extract(hdr.MIH);
        
        transition select(hdr.flag.flag) {
			6: parse_SFH;
            7: parse_SFH;
			default : accept;
		}
    }

	state parse_SFH {
		packet.extract(hdr.SFH);
		transition accept;
	}
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {

        //parsed headers have to be added again into the packet.
        packet.emit(hdr.ethernet);
        packet.emit(hdr.CPU);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.tcp);
        packet.emit(hdr.udp);
        packet.emit(hdr.MIH);
        packet.emit(hdr.SFH);
    }
}
