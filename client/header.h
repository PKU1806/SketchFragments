#ifndef __HEADER_H__
#define __HEADER_H__

#pragma pack(1)

namespace Simulator {

#define MAX_IP_LENGTH 128
#define MAX_PATH_LENGTH 1024

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
