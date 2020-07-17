#pragma pack(1)

struct SF_Header {
    uint16_t mih_switch_id;

    uint64_t mih_timestamp;

    uint16_t sfh_switch_id;

    uint8_t  sfh_sketch_fg;

    uint32_t sfh_fgment_id;

    uint32_t sfh_delay0;
    uint32_t sfh_delay1;
	uint32_t sfh_delay2;
	uint32_t sfh_delay3;
	uint32_t sfh_delay4;
	uint32_t sfh_delay5;
	uint32_t sfh_delay6;
	uint32_t sfh_delay7;
	uint32_t sfh_delay8;
	uint32_t sfh_delay9;
};
