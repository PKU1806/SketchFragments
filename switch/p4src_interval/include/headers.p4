/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

const bit<16> TYPE_IPV4 = 0x0800;
const bit<8> TYPE_TCP = 6;
const bit<8> TYPE_UDP = 17;
const bit<8> TYPE_ICMP = 1;

typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;

header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16>   etherType;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<6>    dscp;
    bit<2>    ecn;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
}

header tcp_t{
    bit<16> srcPort;
    bit<16> dstPort;
    bit<32> seqNo;
    bit<32> ackNo;
    bit<4>  dataOffset;
    bit<1>  MIH_fg;
    bit<1>  SFH_fg;
    bit<1>  SFH_sketch_number;
    bit<1>  res;
    bit<1>  cwr;
    bit<1>  ece;
    bit<1>  urg;
    bit<1>  ack;
    bit<1>  psh;
    bit<1>  rst;
    bit<1>  syn;
    bit<1>  fin;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgentPtr;
}

header udp_t {
	bit<16> srcPort;
	bit<16> dstPort;
	bit<16> length;
	bit<16> checksum;
}

// if using udp protocol ,we will use this one to judge whether exists MIH and SFH
header FLAG_t{
    bit<8> flag; //0x100 for MIH; 0x010 for SFH; 0x001 for sfh_sketch_number
}

//part one of the bringing data
header MIH_t{
    //max interval header
    bit<16>  mih_switch_id;
    bit<48>  mih_timestamp;
}

//sketch fragment header
header SFH_t{
    bit<16> sfh_switch_id;
    bit<32> sfh_fgment_id;

    //sketch fragments: a bucket which contains 10 bins
    bit<32> sfh_delay0;
    bit<32> sfh_delay1;
	bit<32> sfh_delay2;
	bit<32> sfh_delay3;
	bit<32> sfh_delay4;
	bit<32> sfh_delay5;
	bit<32> sfh_delay6;
	bit<32> sfh_delay7;
	bit<32> sfh_delay8;
	bit<32> sfh_delay9;
}

header cpu_t{
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
    bit<8>    protocol;
    bit<16> srcPort;
	bit<16> dstPort;
    bit<48> delay;
    bit<48> interval;
    bit<8> flags;
}

struct metadata {
	bit<16> ipv4_srcPort;
	bit<16> ipv4_dstPort;

    //for timestamp register
    bit<32> timestamp_index0;
    bit<32> timestamp_index1;
    bit<32> timestamp_index2;

    bit<48> timestamp_value0;
    bit<48> timestamp_value1;
    bit<48> timestamp_value2;

    bit<48> MIH_value0;
    bit<48> MIH_value1;
    bit<48> MIH_value2;

    //for max interval register

    bit<48> max_interval_value0;
    bit<48> max_interval_value1;
    bit<48> max_interval_value2;


	bit<16> switch_id;
    bit<48> switch_delay;
    bit<8>  sketch_fg;
    bit<8>  swap_control;

    bit<32> delay_lev;
    bit<48> previous_ingress_global_timestamp;
    bit<48> interval;

    bit<32> random_number;
    bit<14> ecmp_hash;
    bit<14> ecmp_group_id;
}

struct headers {
    ethernet_t   ethernet;
    cpu_t        CPU;
    ipv4_t       ipv4;
    tcp_t        tcp;
	udp_t        udp;
    FLAG_t       flag;
    MIH_t        MIH;
    SFH_t	     SFH;
}

