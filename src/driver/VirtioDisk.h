//
// Created by 屠昊天 on 2022/5/23.
//


#pragma once
#define VIRTIO_ADDRESS 0x10001000
// device feature bits
#define VIRTIO_BLK_F_RO              5    /* Disk is operate-only */
#define VIRTIO_BLK_F_SCSI            7    /* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE     11    /* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ             12    /* support more than one vq */
#define VIRTIO_F_ANY_LAYOUT         27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX     29
#define QUEUE_NUM 8

#define VIRTIO_BLK_T_IN  0 // read the disk
#define VIRTIO_BLK_T_OUT 1 // write the disk

#define VRING_DESC_F_READ 0 // device read
#define VRING_DESC_F_NEXT  1 // chained with another descriptor
#define VRING_DESC_F_WRITE 2 // device writes (vs read)

#include <cstddef>
#include <cstdint>

struct VirtioQueueDescription {
    uint64_t address;
    uint32_t length;
    uint16_t flags;
    uint16_t next;
};

struct VirtioQueueAvailable {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[QUEUE_NUM];
    uint16_t next;
};

struct VirtioQueueUsedElement {
    uint32_t id;
    uint32_t length;
};

struct VirtioQueueUsed {
    uint16_t flags;
    uint16_t idx;
    VirtioQueueUsedElement ring[QUEUE_NUM];
};

struct VirtioBlockRequest {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
};

enum class DiskOperationType {
    READ,
    WRITE
};


struct DiskOperationRequest {
    DiskOperationType type;
    bool valid;
    size_t sector;
    void* address;
    size_t length;
};

struct MMIOStatus {
    bool acknowledge: 1;
    bool driver: 1;
    bool driverOK: 1;
    bool featureOK: 1;
    uint8_t reserve1: 3;
    bool deviceNeedsReset: 1;
    uint8_t reserve: 7;
    bool failed: 1;
} __attribute__((packed));
static_assert(sizeof(MMIOStatus) == 2);

//struct MMIODeviceRegisters {
//    /* 0x000 */ uint32_t magicValue;
//    /* 0x004 */ uint32_t version;
//    /* 0x008 */ uint32_t deviceId;
//    /* 0x00c */ uint32_t vendorId;
//    /* 0x010 */ uint32_t deviceFeatures;
//    /* 0x014 */ uint8_t deviceFeaturesSel[12];
//    /* 0x020 */ uint32_t driverFeatures;
//    /* 0x024 */ uint32_t driverFeaturesSel;
//    /* 0x028 */ uint64_t pageSize;
//    /* 0x030 */ uint32_t queueSel;
//    /* 0x034 */ uint32_t queueNumMax;
//    /* 0x038 */ uint32_t queueNum;
//    /* 0x03c */ uint32_t queueAlign;
//    /* 0x040 */ uint64_t queuePFN;
//    /* 0x048 */ uint64_t reserve;
//    /* 0x050 */ uint8_t queueNotify;
//    /* 0x051 */ uint8_t reserve1[15];
//    /* 0x060 */ uint32_t interruptStatus;
//    /* 0x064 */ uint8_t interruptACK[12];
//    /* 0x070 */ MMIOStatus status;
//}__attribute__((packed));
//static_assert(sizeof(MMIODeviceRegisters) == 0x72);


class VirtioDisk {
private:
    void* pages;
    VirtioQueueDescription* descriptions;
    VirtioQueueAvailable* available;
    VirtioQueueUsed* used;

    bool free[QUEUE_NUM];
    uint16_t usedIdx;
    VirtioBlockRequest ops[QUEUE_NUM];

    struct {
        DiskOperationRequest* request;
        uint8_t status;
    } info[QUEUE_NUM];

    int allocateDescription();

    void freeDescription(int index);

    void freeDescriptionChain(int index);

    bool allocate3Description(int* idx);

public:
    VirtioDisk();
};

extern VirtioDisk PrimaryDisk;