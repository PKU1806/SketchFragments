
/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

//My includes
#include "./headers.p4"
#include "./parsers.p4"




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

	/******************* inherited code starts here       ************************/
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
			meta.ipv4_srcPort,
			meta.ipv4_dstPort,
			hdr.ipv4.protocol
			},
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

	/******************* inherited code ends here       ************************/



	/******** log code starts here*******/


	/******** log code ends here*******/

	apply
	{   
		if (hdr.ipv4.isValid()&&hdr.ipv4.ttl > 1) {
			
			switch (ipv4_lpm.apply().action_run){
				ecmp_group:{
					ecmp_group_to_nhop.apply();
				}
			}
		}
		else{
			drop();
		}
		
	}
	
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
				 inout metadata meta,
				 inout standard_metadata_t standard_metadata)
{
	

	apply
	{
		if(hdr.ipv4.isValid()&&hdr.udp.isValid()&&standard_metadata.instance_type ==0&&hdr.ipv4.ttl > 1){
			if(!hdr.flag.isValid()){
				hdr.flag.setValid();
			}
			hdr.flag.ingress_timestamp=standard_metadata.ingress_global_timestamp;
			hdr.flag.egress_timestamp=standard_metadata.egress_global_timestamp;
			
		
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

		

		update_checksum_with_payload(
			hdr.flag.isValid(),
			{hdr.ipv4.srcAddr,
			 hdr.ipv4.dstAddr,
			 8w0,
			 hdr.ipv4.protocol,
			 hdr.udp.length,
			 hdr.udp.srcPort,
			 hdr.udp.dstPort,
			 hdr.udp.length,
			 hdr.flag},
			hdr.udp.checksum,
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
