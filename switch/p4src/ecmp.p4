/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

//My includes
#include "include/headers.p4"
#include "include/parsers.p4"

#define BUCKET_NUM 10000
#define BIN_NUM 10
#define BIN_CELL_BIT_WIDTH 32
#define LOW_BOUND 1
#define HIGH_BOUND 30

#define ARRAY_REGISTER(num) register<bit<BIN_CELL_BIT_WIDTH>>(BUCKET_NUM * BIN_NUM) array##num

//6crc
//this is for sketch
#define COMPUTE_ARRAY_HASH(num) hash(meta.array_index##num,\
             HashAlgorithm.crc32_custom,\
             (bit<16>)0,\
             {hdr.ipv4.srcAddr,\
              hdr.ipv4.dstAddr,\
              hdr.tcp.srcPort,\
              hdr.tcp.dstPort,\
              hdr.ipv4.protocol},\
             (bit<32>)BUCKET_NUM);

//this is for timestamps
#define COMPUTE_TIMESTAMP_HASH(num)  hash(meta.timestamp_index##num,\
            HashAlgorithm.crc32_custom, \
            (bit<16>)0,\
            {hdr.ipv4.srcAddr,     \
            hdr.ipv4.dstAddr,     \
            hdr.tcp.srcPort,      \
            hdr.tcp.dstPort,      \
            hdr.ipv4.protocol},   \
            (bit<32>)BUCKET_NUM);       \

//1 crc32 custom
//this is for bring sketch fragments
#define COMPUTE_SFH_HASH hash(meta.SFH_index0,\
             HashAlgorithm.crc32,\
             (bit<16>)0,\
             {hdr.ipv4.srcAddr,\
              hdr.ipv4.dstAddr,\
              hdr.tcp.srcPort,\
              hdr.tcp.dstPort,\
              hdr.ipv4.protocol},\
             (bit<32>)(BUCKET_NUM*3));

//group0
ARRAY_REGISTER(0);
ARRAY_REGISTER(1);
ARRAY_REGISTER(2);

//group1
ARRAY_REGISTER(3);
ARRAY_REGISTER(4);
ARRAY_REGISTER(5);

//timestamps
register<bit<48>>(BUCKET_NUM) timestamp_array0;
register<bit<48>>(BUCKET_NUM) timestamp_array1;
register<bit<48>>(BUCKET_NUM) timestamp_array2;

//control plane used
register<bit<1>>(1) sketch_flag;
register<bit<1>>(1) swap_control;

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta)
{
    apply {}
}

/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata)
{

    action predispose(){
        COMPUTE_SFH_HASH
        if(meta.SFH_index0<10000){
            meta.SFH_target_bucket=meta.SFH_index0;
            meta.SFH_target_array=0;
        }
        else if(meta.SFH_index0<20000){
            meta.SFH_target_bucket=meta.SFH_index0-10000;
            meta.SFH_target_array=1;
        }
        else{
            meta.SFH_target_bucket=meta.SFH_index0-20000;
            meta.SFH_target_array=2;
        }

        if(meta.sketch_flag==0){
            meta.SFH_target_array=meta.SFH_target_array+3;
        }
        return ;
    }

    action update_using_sketch0(){
        

       array0.read(meta.tmp00,meta.SFH_target_bucket*BIN_NUM+0);
       array0.read(meta.tmp01,meta.SFH_target_bucket*BIN_NUM+1);
       array0.read(meta.tmp02,meta.SFH_target_bucket*BIN_NUM+2);
       array0.read(meta.tmp03,meta.SFH_target_bucket*BIN_NUM+3);
       array0.read(meta.tmp04,meta.SFH_target_bucket*BIN_NUM+4);
       array0.read(meta.tmp05,meta.SFH_target_bucket*BIN_NUM+5);
       array0.read(meta.tmp06,meta.SFH_target_bucket*BIN_NUM+6);
       array0.read(meta.tmp07,meta.SFH_target_bucket*BIN_NUM+7);
       array0.read(meta.tmp08,meta.SFH_target_bucket*BIN_NUM+8);
       array0.read(meta.tmp09,meta.SFH_target_bucket*BIN_NUM+9);

       array1.read(meta.tmp10,meta.SFH_target_bucket*BIN_NUM+0);
       array1.read(meta.tmp11,meta.SFH_target_bucket*BIN_NUM+1);
       array1.read(meta.tmp12,meta.SFH_target_bucket*BIN_NUM+2);
       array1.read(meta.tmp13,meta.SFH_target_bucket*BIN_NUM+3);
       array1.read(meta.tmp14,meta.SFH_target_bucket*BIN_NUM+4);
       array1.read(meta.tmp15,meta.SFH_target_bucket*BIN_NUM+5);
       array1.read(meta.tmp16,meta.SFH_target_bucket*BIN_NUM+6);
       array1.read(meta.tmp17,meta.SFH_target_bucket*BIN_NUM+7);
       array1.read(meta.tmp18,meta.SFH_target_bucket*BIN_NUM+8);
       array1.read(meta.tmp19,meta.SFH_target_bucket*BIN_NUM+9);

       array2.read(meta.tmp20,meta.SFH_target_bucket*BIN_NUM+0);
       array2.read(meta.tmp21,meta.SFH_target_bucket*BIN_NUM+1);
       array2.read(meta.tmp22,meta.SFH_target_bucket*BIN_NUM+2);
       array2.read(meta.tmp23,meta.SFH_target_bucket*BIN_NUM+3);
       array2.read(meta.tmp24,meta.SFH_target_bucket*BIN_NUM+4);
       array2.read(meta.tmp25,meta.SFH_target_bucket*BIN_NUM+5);
       array2.read(meta.tmp26,meta.SFH_target_bucket*BIN_NUM+6);
       array2.read(meta.tmp27,meta.SFH_target_bucket*BIN_NUM+7);
       array2.read(meta.tmp28,meta.SFH_target_bucket*BIN_NUM+8);
       array2.read(meta.tmp29,meta.SFH_target_bucket*BIN_NUM+9);

       if(meta.SFH_target_array==0){
           hdr.SFH.delay0=meta.tmp00;
           hdr.SFH.delay1=meta.tmp01;
           hdr.SFH.delay2=meta.tmp02;
           hdr.SFH.delay3=meta.tmp03;
           hdr.SFH.delay4=meta.tmp04;
           hdr.SFH.delay5=meta.tmp05;
           hdr.SFH.delay6=meta.tmp06;
           hdr.SFH.delay7=meta.tmp07;
           hdr.SFH.delay8=meta.tmp08;
           hdr.SFH.delay9=meta.tmp09;
       }
       else if(meta.SFH_target_array==1){
           hdr.SFH.delay0=meta.tmp10;
           hdr.SFH.delay1=meta.tmp11;
           hdr.SFH.delay2=meta.tmp12;
           hdr.SFH.delay3=meta.tmp13;
           hdr.SFH.delay4=meta.tmp14;
           hdr.SFH.delay5=meta.tmp15;
           hdr.SFH.delay6=meta.tmp16;
           hdr.SFH.delay7=meta.tmp17;
           hdr.SFH.delay8=meta.tmp18;
           hdr.SFH.delay9=meta.tmp19;
       }
       else{
           hdr.SFH.delay0=meta.tmp20;
           hdr.SFH.delay1=meta.tmp21;
           hdr.SFH.delay2=meta.tmp22;
           hdr.SFH.delay3=meta.tmp23;
           hdr.SFH.delay4=meta.tmp24;
           hdr.SFH.delay5=meta.tmp25;
           hdr.SFH.delay6=meta.tmp26;
           hdr.SFH.delay7=meta.tmp27;
           hdr.SFH.delay8=meta.tmp28;
           hdr.SFH.delay9=meta.tmp29;
       }
    }

    action update_using_sketch1(){
    
       array3.read(meta.tmp00,meta.SFH_target_bucket*BIN_NUM+0);
       array3.read(meta.tmp01,meta.SFH_target_bucket*BIN_NUM+1);
       array3.read(meta.tmp02,meta.SFH_target_bucket*BIN_NUM+2);
       array3.read(meta.tmp03,meta.SFH_target_bucket*BIN_NUM+3);
       array3.read(meta.tmp04,meta.SFH_target_bucket*BIN_NUM+4);
       array3.read(meta.tmp05,meta.SFH_target_bucket*BIN_NUM+5);
       array3.read(meta.tmp06,meta.SFH_target_bucket*BIN_NUM+6);
       array3.read(meta.tmp07,meta.SFH_target_bucket*BIN_NUM+7);
       array3.read(meta.tmp08,meta.SFH_target_bucket*BIN_NUM+8);
       array3.read(meta.tmp09,meta.SFH_target_bucket*BIN_NUM+9);

       array4.read(meta.tmp10,meta.SFH_target_bucket*BIN_NUM+0);
       array4.read(meta.tmp11,meta.SFH_target_bucket*BIN_NUM+1);
       array4.read(meta.tmp12,meta.SFH_target_bucket*BIN_NUM+2);
       array4.read(meta.tmp13,meta.SFH_target_bucket*BIN_NUM+3);
       array4.read(meta.tmp14,meta.SFH_target_bucket*BIN_NUM+4);
       array4.read(meta.tmp15,meta.SFH_target_bucket*BIN_NUM+5);
       array4.read(meta.tmp16,meta.SFH_target_bucket*BIN_NUM+6);
       array4.read(meta.tmp17,meta.SFH_target_bucket*BIN_NUM+7);
       array4.read(meta.tmp18,meta.SFH_target_bucket*BIN_NUM+8);
       array4.read(meta.tmp19,meta.SFH_target_bucket*BIN_NUM+9);

       array5.read(meta.tmp20,meta.SFH_target_bucket*BIN_NUM+0);
       array5.read(meta.tmp21,meta.SFH_target_bucket*BIN_NUM+1);
       array5.read(meta.tmp22,meta.SFH_target_bucket*BIN_NUM+2);
       array5.read(meta.tmp23,meta.SFH_target_bucket*BIN_NUM+3);
       array5.read(meta.tmp24,meta.SFH_target_bucket*BIN_NUM+4);
       array5.read(meta.tmp25,meta.SFH_target_bucket*BIN_NUM+5);
       array5.read(meta.tmp26,meta.SFH_target_bucket*BIN_NUM+6);
       array5.read(meta.tmp27,meta.SFH_target_bucket*BIN_NUM+7);
       array5.read(meta.tmp28,meta.SFH_target_bucket*BIN_NUM+8);
       array5.read(meta.tmp29,meta.SFH_target_bucket*BIN_NUM+9);

       if(meta.SFH_target_array==3){
           hdr.SFH.delay0=meta.tmp00;
           hdr.SFH.delay1=meta.tmp01;
           hdr.SFH.delay2=meta.tmp02;
           hdr.SFH.delay3=meta.tmp03;
           hdr.SFH.delay4=meta.tmp04;
           hdr.SFH.delay5=meta.tmp05;
           hdr.SFH.delay6=meta.tmp06;
           hdr.SFH.delay7=meta.tmp07;
           hdr.SFH.delay8=meta.tmp08;
           hdr.SFH.delay9=meta.tmp09;
       }
       else if(meta.SFH_target_array==4){
           hdr.SFH.delay0=meta.tmp10;
           hdr.SFH.delay1=meta.tmp11;
           hdr.SFH.delay2=meta.tmp12;
           hdr.SFH.delay3=meta.tmp13;
           hdr.SFH.delay4=meta.tmp14;
           hdr.SFH.delay5=meta.tmp15;
           hdr.SFH.delay6=meta.tmp16;
           hdr.SFH.delay7=meta.tmp17;
           hdr.SFH.delay8=meta.tmp18;
           hdr.SFH.delay9=meta.tmp19;
       }
       else{
           hdr.SFH.delay0=meta.tmp20;
           hdr.SFH.delay1=meta.tmp21;
           hdr.SFH.delay2=meta.tmp22;
           hdr.SFH.delay3=meta.tmp23;
           hdr.SFH.delay4=meta.tmp24;
           hdr.SFH.delay5=meta.tmp25;
           hdr.SFH.delay6=meta.tmp26;
           hdr.SFH.delay7=meta.tmp27;
           hdr.SFH.delay8=meta.tmp28;
           hdr.SFH.delay9=meta.tmp29;
       }
    }

    table update_SFH{
        key={
            meta.sketch_flag:exact;
        }
        actions={
            NoAction;
            update_using_sketch0;
            update_using_sketch1;
        }
        size=256;
        default_action=NoAction;
    }

    action drop(){
        mark_to_drop();
    }

    action ecmp_group(bit<14> ecmp_group_id, bit<16> num_nhops)
    {
        hash(meta.ecmp_hash,
            HashAlgorithm.crc16,
            (bit<1>)0,
            {hdr.ipv4.srcAddr,
            hdr.ipv4.dstAddr,
            hdr.tcp.srcPort,
            hdr.tcp.dstPort,
            hdr.ipv4.protocol},
            num_nhops);

        meta.ecmp_group_id = ecmp_group_id;
    }

    action set_nhop(macAddr_t dstAddr, egressSpec_t port)
    {
        //set the src mac address as the previous dst, this is not correct right?
        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;
        //set the destination mac address that we got from the match in the table
        hdr.ethernet.dstAddr = dstAddr;
        //set the output port that we also get from the table
        standard_metadata.egress_spec = port;
        //decrease ttl by 1
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;
    }

    table ecmp_group_to_nhop
    {
        key = {
            meta.ecmp_group_id : exact;
            meta.ecmp_hash : exact;
        }
        actions = {
            drop;
            set_nhop;
        }
        size = 1024;
    }

    table ipv4_lpm
    {
        key = {
            hdr.ipv4.dstAddr : lpm;
        }
        actions = {
           set_nhop;
        ecmp_group;
        drop;
        }
        size = 1024;
        default_action = drop;
    }

    action _bring_switch_id(bit<16> switch_id){
        hdr.SFH.switch_id=switch_id;
    }

    table bring_switch_id{
        key={

        }
        actions={
            _bring_switch_id;
            NoAction;
        }
        size=256;
        default_action=NoAction;
    }

    action update_SFH_timestamp()
    {
        COMPUTE_TIMESTAMP_HASH(0)
        COMPUTE_TIMESTAMP_HASH(1)
        COMPUTE_TIMESTAMP_HASH(2)
        
        timestamp_array0.read(meta.timestamp_value0,meta.timestamp_index0);
        timestamp_array1.read(meta.timestamp_value1,meta.timestamp_index1);
        timestamp_array2.read(meta.timestamp_value2,meta.timestamp_index2);

        meta.timestamp_value0=standard_metadata.ingress_global_timestamp-meta.timestamp_value0;
        meta.timestamp_value1=standard_metadata.ingress_global_timestamp-meta.timestamp_value1;
        meta.timestamp_value2=standard_metadata.ingress_global_timestamp-meta.timestamp_value2;

        if(meta.timestamp_value0>hdr.SFH.max_bucket_interval){
            hdr.SFH.max_interval_switch_id=hdr.SFH.switch_id;
            hdr.SFH.max_bucket_interval=meta.timestamp_value0;
        }
        if(meta.timestamp_value1>hdr.SFH.max_bucket_interval){
            hdr.SFH.max_interval_switch_id=hdr.SFH.switch_id;
            hdr.SFH.max_bucket_interval=meta.timestamp_value1;
        }
        if(meta.timestamp_value2>hdr.SFH.max_bucket_interval){
            hdr.SFH.max_interval_switch_id=hdr.SFH.switch_id;
            hdr.SFH.max_bucket_interval=meta.timestamp_value2;
        }
    }


    apply
    {
        if (hdr.ipv4.isValid())
        {
            if (hdr.SFH.isValid())
            {
                
                swap_control.read(meta.swap_control,0);//0 bring-able 1 forbidden
                if (meta.swap_control==0&&hdr.SFH.switch_id <= 0 )
                {
                    //this packet is vacant 

                    update_SFH_timestamp();


                    //the probility allows
                    random(meta.random_number, (bit<32>)LOW_BOUND, (bit<32>)HIGH_BOUND);
                    if (meta.random_number == (LOW_BOUND + 1))
                    {
                        bring_switch_id.apply();
                        sketch_flag.read(meta.sketch_flag,0);
                        hdr.SFH.sketch_flag=meta.sketch_flag;
                        //hash suspend
                        predispose();
                        update_SFH.apply();
                    }
                }

            }
            switch (ipv4_lpm.apply().action_run)
            {
                ecmp_group:
                {
                    ecmp_group_to_nhop.apply();
                }
            }
        }
    }
    /*
    apply
    {
        //do nothing
    }*/
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata)
{
    //egress part successfully compiled and refigured into semi-cu
    //time:2020-07-13-15:56 
    //tested by @leo
    //function:we can only get the egress time in this part ,so we implement the insert of delay here
    //first get the level of bin
    //then insert
    //sketch flag will be init and modified by controller
    //the insertion of timestamp can be moved to previous part

    //this action get the corresponding lev of bucket
    action get_delay_lev()
    {
        //whether p4 support switch && > ?
        if (meta.switch_delay < 10){
            meta.delay_lev = 0;
            return;
        }
        else if (meta.switch_delay < 20){
            meta.delay_lev = 1;
            return;
        }
        else if (meta.switch_delay < 30){
            meta.delay_lev = 2;
            return;
        }
        else if (meta.switch_delay < 40){
            meta.delay_lev = 3;
            return;
        }
        else if (meta.switch_delay < 50){
            meta.delay_lev = 4;
            return;
        }
        else if (meta.switch_delay < 60){
            meta.delay_lev = 5;
            return;
        }
        else if (meta.switch_delay < 70){
            meta.delay_lev = 6;
            return;
        }
        else if (meta.switch_delay < 80){
            meta.delay_lev = 7;
            return;
        }
        else if (meta.switch_delay < 90){
            meta.delay_lev = 8;
            return;
        }
        else {
            meta.delay_lev = 9;
            return;
        }
    }

    //semi-CU-sketch    
    //update sketch 0 : array0\1\2
    action update_sketch0()
    {
        COMPUTE_ARRAY_HASH(0)
        COMPUTE_ARRAY_HASH(1)
        COMPUTE_ARRAY_HASH(2)

        bit<32>tmp_min=0x5fffffff;

        array0.read(meta.array_value0, (meta.array_index0 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value0){
            tmp_min=meta.array_value0;
            meta.array_value0 = meta.array_value0 + 1;
        }

        array1.read(meta.array_value1, (meta.array_index1 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value1){
            tmp_min=meta.array_value1;
            meta.array_value1 = meta.array_value1 + 1;
        }

        array2.read(meta.array_value2, (meta.array_index2 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value2){
            tmp_min=meta.array_value2;
            meta.array_value2 = meta.array_value2 + 1;
        }

        array0.write((meta.array_index0 * BIN_NUM + meta.delay_lev), meta.array_value0);
        array1.write((meta.array_index1 * BIN_NUM + meta.delay_lev), meta.array_value1);
        array2.write((meta.array_index2 * BIN_NUM + meta.delay_lev), meta.array_value2);
    }

    //update sketch 1 : array3\4\5
    action update_sketch1()
    {
        COMPUTE_ARRAY_HASH(3)
        COMPUTE_ARRAY_HASH(4)
        COMPUTE_ARRAY_HASH(5)

        bit<32>tmp_min=0x5fffffff;

        array3.read(meta.array_value3, (meta.array_index3 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value3){
            tmp_min=meta.array_value3;
            meta.array_value3 = meta.array_value3 + 1;
        }

        array4.read(meta.array_value4, (meta.array_index4 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value4){
            tmp_min=meta.array_value4;
            meta.array_value4 = meta.array_value4 + 1;
        }

        array5.read(meta.array_value5, (meta.array_index5 * BIN_NUM + meta.delay_lev));
        if(tmp_min>=meta.array_value5){
            tmp_min=meta.array_value5;
            meta.array_value5 = meta.array_value5 + 1;
        }

        array3.write((meta.array_index3 * BIN_NUM + meta.delay_lev), meta.array_value3);
        array4.write((meta.array_index4 * BIN_NUM + meta.delay_lev), meta.array_value4);
        array5.write((meta.array_index5 * BIN_NUM + meta.delay_lev), meta.array_value5);
    }

    action update_timestamp(){
        
        //this has been computed in ingress part
        
        timestamp_array0.write(meta.timestamp_index0,standard_metadata.ingress_global_timestamp);
        timestamp_array1.write(meta.timestamp_index1,standard_metadata.ingress_global_timestamp);
        timestamp_array2.write(meta.timestamp_index2,standard_metadata.ingress_global_timestamp);
    }

    table update_sketch
    {
        key = {
            meta.sketch_flag : exact;
        }
        actions = {
            update_sketch0;
            update_sketch1;
            NoAction;
        }
        size = 256;
        default_action = NoAction;
    }


    apply
    {
        if(hdr.ipv4.isValid()){
            update_timestamp();
            meta.switch_delay = standard_metadata.egress_global_timestamp-standard_metadata.ingress_global_timestamp;
            get_delay_lev();
            sketch_flag.read(meta.sketch_flag,0);
            update_sketch.apply();
        }
        
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta)
{
    apply
    {
        update_checksum(
            hdr.ipv4.isValid(),
            {hdr.ipv4.version,
             hdr.ipv4.ihl,
             hdr.ipv4.dscp,
             hdr.ipv4.ecn,
             hdr.ipv4.totalLen,
             hdr.ipv4.identification,
             hdr.ipv4.flags,
             hdr.ipv4.fragOffset,
             hdr.ipv4.ttl,
             hdr.ipv4.protocol,
             hdr.ipv4.srcAddr,
             hdr.ipv4.dstAddr},
            hdr.ipv4.hdrChecksum,
            HashAlgorithm.csum16);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

//switch architecture
V1Switch(
    MyParser(),
    MyVerifyChecksum(),
    MyIngress(),
    MyEgress(),
    MyComputeChecksum(),
    MyDeparser()) main;
