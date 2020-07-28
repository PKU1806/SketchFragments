#ifndef __HEADER_H__
#define __HEADER_H__

#pragma pack(1)

namespace Simulator {

#define MAX_IP_LENGTH 128
#define MAX_PATH_LENGTH 1024
#define MAX_HOST_NUM 1024

const int sw_num = 10;
const int ar_num = 3;
const int bu_num = 64;

struct MIH_Header {
    uint16_t switch_id;
    uint64_t tim_epoch;
	uint8_t  exists_fg;
};

struct SFH_Header {
    uint16_t switch_id;
    uint8_t  sketch_fg;
    uint32_t fgment_id;
    uint32_t delay[10];
};

struct COM_Header {
	MIH_Header mih;
	SFH_Header sfh;
};

}

#endif
