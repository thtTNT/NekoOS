//
// Created by 屠昊天 on 2022/5/23.
//

#include "VirtioDisk.h"
#include "../driver/render.h"

VirtioDisk PrimaryDisk;  // NOLINT(cert-err58-cpp)

VirtioDisk::VirtioDisk() {
    auto registers = (MMIODeviceRegisters*) VIRTIO_ADDRESS;

    if (registers->magicValue != 0x74726976 || registers->version != 1 || registers->deviceId != 2 || registers->vendorId != 0x554d4551){
        return;
    }

    Render::print("Detect hard disk!\n");
}
