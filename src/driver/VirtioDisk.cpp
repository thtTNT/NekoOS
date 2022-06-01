//
// Created by 屠昊天 on 2022/5/23.
//

#include "VirtioDisk.h"
#include "../memory/Memory.h"
#include "../driver/render.h"
#include "../memory/PageFrameAllocator.h"
#include "../utils/assert.h"

VirtioDisk PrimaryDisk;  // NOLINT(cert-err58-cpp)

enum Registers : uint64_t {
    REGISTER_MAGIC_VALUE = 0x000,
    REGISTER_VERSION = 0x004,
    REGISTER_DEVICE_ID = 0x008,
    REGISTER_VENDOR_ID = 0x00c,
    REGISTER_DEVICE_FEATURES = 0x010,
    REGISTER_PAGE_SIZE = 0x28,
    REGISTER_QUEUE_SEL = 0x030,
    REGISTER_QUEUE_NUM_MAX = 0x034,
    REGISTER_QUEUE_NUM = 0x038,
    REGISTER_QUEUE_PFN = 0x040,
    REGISTER_QUEUE_NOTIFY = 0x50,
    REGISTER_INTERRUPT_STATUS = 0x60,
    REGISTER_INTERRUPT_ACK = 0x64,
    REGISTER_STATUS = 0x70
};

static uint32_t readRegister(Registers reg) {
    return *(uint32_t*) (VIRTIO_ADDRESS + reg);
}

static void writeRegister(Registers reg, uint32_t value) {
    *(uint32_t*) (VIRTIO_ADDRESS + reg) = value;
}

VirtioDisk::VirtioDisk() {
    if (readRegister(REGISTER_MAGIC_VALUE) != 0x74726976 ||
        readRegister(REGISTER_VERSION) != 1 ||
        readRegister(REGISTER_DEVICE_ID) != 2 ||
        readRegister(REGISTER_VENDOR_ID) != 0x554d4551) {
        return;
    }
    Render::print("Detect hard disk!\n");

    auto status = MMIOStatus{};

    status.acknowledge = true;
    writeRegister(REGISTER_STATUS, *(uint32_t*) &status);

    status.driver = true;
    writeRegister(REGISTER_STATUS, *(uint32_t*) &status);

    // negotiate features
    uint32_t features = readRegister(REGISTER_DEVICE_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
    writeRegister(REGISTER_DEVICE_FEATURES, features);

    status.featureOK = true;
    writeRegister(REGISTER_STATUS, *(uint32_t*) &status);

    status.driverOK = true;
    writeRegister(REGISTER_STATUS, *(uint32_t*) &status);


    writeRegister(REGISTER_PAGE_SIZE, PAGE_SIZE);

    // initialize queue 0
    writeRegister(REGISTER_QUEUE_SEL, 0);
    uint32_t queueMax = readRegister(REGISTER_QUEUE_NUM_MAX);
    if (queueMax == 0) {
        Render::print("Hard disk has no queue 0!\n");
        return;
    }
    if (queueMax < QUEUE_NUM) {
        Render::print("Hard disk max queue is too short!\n");
        return;
    }
    writeRegister(REGISTER_QUEUE_NUM, QUEUE_NUM);

    // Allocate page
    this->pages = GlobalPageFrameAllocator.requestPages(2);
    memset(this->pages, 0, PAGE_SIZE * 2);
    writeRegister(REGISTER_QUEUE_PFN, (uint64_t) this->pages / PAGE_SIZE);

    this->descriptions = (VirtioQueueDescription*) this->pages;
    this->available = (VirtioQueueAvailable*) ((size_t) this->pages + QUEUE_NUM * sizeof(VirtioQueueDescription));
    this->used = (VirtioQueueUsed*) ((size_t) this->pages + PAGE_SIZE);
    assert((size_t) this->available < (size_t) this->used);

    for (bool &i: this->free) {
        i = true;
    }

    Render::print("Hard Disk load successfully!\n");
}

void VirtioDisk::operate(DiskOperationRequest &request) {
    int idx[3];
    while (!allocate3Description(idx)) {}

    VirtioBlockRequest* buf = &this->ops[idx[0]];
    if (request.type == DiskOperationType::READ) {
        buf->type = VIRTIO_BLK_T_IN;
    }
    if (request.type == DiskOperationType::WRITE) {
        buf->type = VIRTIO_BLK_T_OUT;
    }
    buf->sector = request.sector;
    buf->reserved = 0;

    {
        auto description = &this->descriptions[idx[0]];
        description->address = (uint64_t) buf;
        description->length = sizeof(VirtioBlockRequest);
        description->flags = VRING_DESC_F_NEXT;
        description->next = idx[1];
    }

    {
        auto description = &this->descriptions[idx[1]];
        description->address = (uint64_t) request.address;
        description->length = request.length;
        if (request.type == DiskOperationType::READ) {
            description->flags = VRING_DESC_F_WRITE;
        }
        if (request.type == DiskOperationType::WRITE) {
            description->flags = VRING_DESC_F_READ;
        }
        description->flags |= VRING_DESC_F_NEXT;
        description->next = idx[2];
    }

    {
        auto description = &this->descriptions[idx[2]];
        description->address = (uint64_t) &this->info[idx[0]].status;
        description->length = 1;
        description->flags = VRING_DESC_F_WRITE;
        description->next = 0;
    }

    this->info[idx[0]].status = 0xff;
    this->info[idx[0]].request = &request;

    this->available->ring[this->available->idx % QUEUE_NUM] = idx[0];

    __sync_synchronize();

    this->available->idx++;

    __sync_synchronize();

    writeRegister(REGISTER_QUEUE_NOTIFY, 0);

    while (!request.valid) {};

    this->info[idx[0]].request = nullptr;
    this->freeDescriptionChain(idx[0]);
}

bool VirtioDisk::allocate3Description(int* idx) {
    for (int i = 0; i < 3; i++) {
        idx[i] = allocateDescription();
        if (idx[i] < 0) {
            for (int j = 0; j < i; j++) {
                this->freeDescription(idx[j]);
            }
            return false;
        }
    }
    return true;
}

int VirtioDisk::allocateDescription() {
    for (int i = 0; i < QUEUE_NUM; i++) {
        if (this->free[i]) {
            this->free[i] = false;
            return i;
        }
    }
    return -1;
}

void VirtioDisk::freeDescription(int index) {
    assert(index < QUEUE_NUM);
    assert(!this->free[index]);
    this->descriptions[index].address = (uint64_t) nullptr;
    this->descriptions[index].length = 0;
    this->descriptions[index].flags = 0;
    this->descriptions[index].next = 0;
    this->free[index] = true;
}

void VirtioDisk::freeDescriptionChain(int index) {
    while (1) {
        int flag = this->descriptions[index].flags;
        int next = this->descriptions[index].next;
        this->freeDescription(index);
        if (flag & VRING_DESC_F_NEXT) {
            index = next;
        } else {
            break;
        }
    }
}

VirtioDisk &VirtioDisk::operator=(const VirtioDisk &other) {
    if (this == &other) { return *this; }
    memcpy(this, &other, sizeof(VirtioDisk));
    return *this;
}

void VirtioDisk::onInterrupt() {
    // tell the device OS received the interrupt
    writeRegister(REGISTER_INTERRUPT_ACK, readRegister(REGISTER_INTERRUPT_STATUS) & 0x03);

    __sync_synchronize();

    while (this->usedIdx != this->used->idx) {
        __sync_synchronize();
        uint32_t id = this->used->ring[this->usedIdx % QUEUE_NUM].id;
        assert(this->info[id].status == 0);

        auto request = this->info[id].request;
        request->valid = true;

        this->usedIdx++;
    }
}
