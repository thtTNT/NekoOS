//
// Created by 屠昊天 on 2022/5/23.
//


#pragma once
#define VIRTIO_ADDRESS 0x10001000

#include <cstdint>

struct MMIODeviceRegisters {
    /* 0x000 */ uint32_t magicValue;
    /* 0x004 */ uint32_t version;
    /* 0x008 */ uint32_t deviceId;
    /* 0x00c */ uint32_t vendorId;
    /* 0x010 */ uint32_t deviceFeatures;
    /* 0x014 */ uint8_t deviceFeaturesSel[12];
    /* 0x020 */ uint32_t driverFeatures;
    /* 0x024 */ uint8_t driverFeaturesSel[12];
    /* 0x030 */ uint32_t queueSel;
    /* 0x034 */ uint32_t queueNumMax;
    /* 0x038 */ uint8_t queueNum[12];
    /* 0x044 */ uint8_t queueReady[12];
    /* 0x050 */ uint8_t queueNotify[16];
    /* 0x060 */ uint32_t interruptStatus;
    /* 0x064 */ uint8_t interruptACK[12];
    /* 0x070 */ uint8_t status[16];
    /* 0x080 */ uint32_t queueDescLow;
    /* 0x084 */ uint8_t queueDesHigh;
};


class VirtioDisk {
public:
    VirtioDisk();
};

extern VirtioDisk PrimaryDisk;