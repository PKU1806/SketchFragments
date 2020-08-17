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
			default : accept;
		}
    }

    state parse_tcp {
        packet.extract(hdr.tcp);

		meta.ipv4_srcPort = hdr.tcp.srcPort;
		meta.ipv4_dstPort = hdr.tcp.dstPort;

        transition accept;
    }

    state parse_udp {
        packet.extract(hdr.udp);

		meta.ipv4_srcPort = hdr.udp.srcPort;
		meta.ipv4_dstPort = hdr.udp.dstPort;

        transition parse_MIH;
    }

    state parse_MIH {
		packet.extract(hdr.MIH);
        
        transition select(hdr.MIH.sfh_exists_fg) {
			0 : accept;
			1 : parse_SFH;
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

        packet.emit(hdr.ipv4);

        //Only emited if valid
	
        packet.emit(hdr.tcp);

        packet.emit(hdr.udp);

        packet.emit(hdr.MIH);

        packet.emit(hdr.SFH);
    }
}