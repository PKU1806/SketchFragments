/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

const bit<16> TYPE_IPV4 = 0x0800;

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
    bit<4>  res;
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

//sketch fragment header
header SFH_t{
	//bit<8> cnt;//historically inherited
	//bit<8> protocol;//historically inherited

    //the followings are the newly defined domains
    //comply with the sequence of slide

    //part 0:
    bit<16> switch_id;//switch id
    bit<1> sketch_flag;//0 stands for group0,vice versa
    bit<3> padding;
    bit<4> hash_id;

    //sketch fragments: a bucket which contains 10 bins
    bit<32> delay0;
    bit<32> delay1;
	bit<32> delay2;
	bit<32> delay3;
	bit<32> delay4;
	bit<32> delay5;
	bit<32> delay6;
	bit<32> delay7;
	bit<32> delay8;
	bit<32> delay9;

    //part 1 
    bit<16> max_interval_switch_id;
    bit<48> max_bucket_interval;//modified from 32 to 48


}

struct metadata {
    //for sketch register
    bit<32> array_index1;
    bit<32> array_index2;
    bit<32> array_index0;
    bit<32> array_index3;
    bit<32> array_index4;
    bit<32> array_index5;

    bit<32> array_value0;
    bit<32> array_value1;
    bit<32> array_value2;
    bit<32> array_value3;
    bit<32> array_value4;
    bit<32> array_value5;
    
    //for int hash
    //bit<32> SFH_index1;
    //bit<32> SFH_index2;
    //bit<32> SFH_index3;
    bit<32> SFH_index0;

    bit<32> timestamp_index0;    //for timestamp register
    bit<32> timestamp_index1;    //for timestamp register
    bit<32> timestamp_index2;    //for timestamp register

    bit<48> timestamp_value0;
    bit<48> timestamp_value1;
    bit<48> timestamp_value2;

    //the above can be refined into at most 4 indexes and 4 values  I suppose
    // but for convenience , we used a lot of them

    bit<48> switch_delay;           //the subst
    bit<1> sketch_flag;             //
    bit<1> swap_control;            //
    bit<32> delay_lev;
    bit<48> previous_ingress_global_timestamp;   //the previous one of timestamp
    bit<48> max_bucket_interval;    //

    bit<3> SFH_target_array;   //the sketch selected
    bit<32> SFH_target_bucket;   //the bucket selected
    bit<32> random_number;          //
    

    bit<14> ecmp_hash;
    bit<14> ecmp_group_id;

    //
    bit<32> tmp00;
    bit<32> tmp01;
    bit<32> tmp02;
    bit<32> tmp03;
    bit<32> tmp04;
    bit<32> tmp05;
    bit<32> tmp06;
    bit<32> tmp07;
    bit<32> tmp08;
    bit<32> tmp09;

    bit<32> tmp10;
    bit<32> tmp11;
    bit<32> tmp12;
    bit<32> tmp13;
    bit<32> tmp14;
    bit<32> tmp15;
    bit<32> tmp16;
    bit<32> tmp17;
    bit<32> tmp18;
    bit<32> tmp19;

    bit<32> tmp20;
    bit<32> tmp21;
    bit<32> tmp22;
    bit<32> tmp23;
    bit<32> tmp24;
    bit<32> tmp25;
    bit<32> tmp26;
    bit<32> tmp27;
    bit<32> tmp28;
    bit<32> tmp29;
}

struct headers {
    ethernet_t   ethernet;
    ipv4_t       ipv4;
    tcp_t        tcp;
    SFH_t	     SFH;
}

