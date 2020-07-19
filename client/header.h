#pragma pack(1)

struct SF_Header {
    uint16_t mih_switch_id;

    uint64_t mih_timestamp;

    uint16_t sfh_switch_id;

    uint8_t  sfh_sketch_fg;

    uint32_t sfh_fgment_id;

    uint32_t sfh_delay[10];
};
