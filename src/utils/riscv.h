//
// Created by 屠昊天 on 2022/5/20.
//


#pragma once

#include <cstdint>

struct SATP {
    uint64_t physicalPageNumber: 44;
    uint16_t asid: 16;
    uint8_t mode: 4;
}__attribute__((packed));

struct STVEC {
    uint8_t mode: 2;
    uint64_t base: 62;
};

struct MTVEC {
    uint8_t mode: 2;
    uint64_t base: 62;
};

struct SIE {
    bool usie: 1;
    bool ssie: 1;
    uint8_t reserve: 2;
    bool utie: 1;
    bool stie: 1;
    uint8_t reserve2: 2;
    bool ueie: 1;
    bool seie: 1;
    uint64_t reserve3: 54;
};

enum MSTATUS_MPP {
    MACHINE = 3,
    SUPERVISOR = 1,
    USER = 0
};

struct MSTATUS {
    bool uie: 1;
    bool sie: 1;
    uint8_t reserve: 1;
    bool mie: 1;
    bool upie: 1;
    bool spie: 1;
    uint8_t reserve1: 1;
    bool mpie: 1;
    bool spp: 1;
    uint8_t reserve2: 2;
    MSTATUS_MPP mpp: 2;
    uint8_t fs: 2;
    uint16_t xs: 2;
    bool mprv: 1;
    bool sum: 1;
    bool mxr: 1;
    bool tvm: 1;
    bool tw: 1;
    bool tsr: 1;
    uint16_t reserve3: 9;
    uint8_t uxl: 2;
    uint16_t sxl: 2;
    uint32_t reserve4: 27;
    bool sd: 1;
}__attribute__((packed));
static_assert(sizeof(MSTATUS) == 0x8);


struct SSTATUS {
    bool uie: 1;
    bool sie: 1;
    uint8_t reserve: 2;
    bool upie: 1;
    bool spie: 1;
    uint8_t reserve1: 2;
    bool spp: 1;
    uint8_t reserve2: 4;
    uint8_t fs: 2;
    uint16_t : 2;
    uint8_t reserve3: 1;
    bool sum: 1;
    bool mxr: 1;
    uint16_t reserve4: 12;
    bool uxl: 2;
    uint32_t reserve5: 29;
    bool sd: 1;
}__attribute__((packed));
static_assert(sizeof(SSTATUS) == 0x8);


struct SCAUSE {
    uint64_t code: 63;
    bool interrupt: 1;

    bool operator==(const SCAUSE &other) {
        return *(uint64_t*) this == *(uint64_t*) &other;
    }

}__attribute__((packed));

static_assert(sizeof(SCAUSE) == 0x8);

#define SCAUSE_INTERRUPT_USER_SOFTWARE_INTERRUPT (SCAUSE{0,true})
#define SCAUSE_INTERRUPT_SUPERVISOR_EXTERNAL_INTERRUPT (SCAUSE{9,true})


static inline SIE r_sie() {
    SIE sie{};
    asm volatile("csrr %0, sie" : "=r" (sie));
    return sie;
}

static inline MSTATUS r_mstatus() {
    uint64_t mstatus;
    asm volatile("csrr %0, mstatus" : "=r" (mstatus));
    return *(MSTATUS*) &mstatus;
}

static inline SCAUSE r_scause() {
    uint64_t scause;
    asm volatile("csrr %0, scause" : "=r" (scause));
    return *(SCAUSE*) &scause;
}

static inline uint64_t r_tp() {
    uint64_t value;
    asm volatile("mv %0, tp" : "=r" (value));
    return value;
}

static inline uint64_t r_mhartid() {
    uint64_t value;
    asm volatile("csrr %0, mhartid" : "=r" (value));
    return value;
}

static inline SSTATUS r_sstatus() {
    uint64_t value;
    asm volatile("csrr %0, sstatus" : "=r" (value));
    return *(SSTATUS*) &value;
}

static inline void w_tp(uint64_t value) {
    asm volatile("mv tp, %0" : : "r" (value));
}

static inline void w_pmpaddr0(uint64_t value) {
    asm volatile("csrw pmpaddr0, %0" : : "r" ((uint64_t) value));
}

static inline void w_pmpcfg0(uint64_t value) {
    asm volatile("csrw pmpcfg0, %0" : : "r" ((uint64_t) value));
}

static inline void w_mepc(void* address) {
    asm volatile("csrw mepc, %0" : : "r" ((uint64_t) address));
}

static inline void w_mstatus(MSTATUS mstatus) {
    asm volatile("csrw mstatus, %0" : : "r" (*(uint64_t*) &mstatus));
}

static inline void w_medeleg(uint64_t value) {
    asm volatile("csrw medeleg, %0" : : "r" (value));
}

static inline void w_mideleg(uint64_t value) {
    asm volatile("csrw mideleg, %0" : : "r" (value));
}

static inline void w_mtvec(MTVEC mtvec) {
    asm volatile("csrw mtvec, %0" : : "r" (*(uint64_t*) &mtvec));
}

static inline void w_stvec(STVEC stvec) {
    asm volatile("csrw stvec, %0" : : "r" (*(uint64_t*) &stvec));
}

static inline void w_satp(SATP satp) {
    asm volatile("csrw satp, %0" : : "r" (*(uint64_t*) &satp));
}

static inline void w_sie(SIE sie) {
    asm volatile("csrw sie, %0" : : "r" (*(uint64_t*) &sie));
}

static inline void w_sstatus(SSTATUS sstatus) {
    asm volatile("csrw sstatus, %0" : : "r" (*(uint64_t*) &sstatus));
}

static inline void setInterrupt(bool value) {
    auto sstatus = r_sstatus();
    sstatus.sie = value;
    w_sstatus(sstatus);
}

// flush the TLB.
static inline void sfence_vma() {
    // the zero, zero means flush all TLB entries.
    asm volatile("sfence.vma zero, zero");
}