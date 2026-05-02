.section .trap
.global trap_entry
.global restore_context

trap_entry:
    addi sp, sp, -16
    sd t0, 0(sp)
    sd t1, 8(sp)

    la t0, currentCtx
    ld t0, 0(t0)

    ld t1, 8(sp)
    sd t1, 48(t0)
    ld t1, 0(sp)
    sd t1, 40(t0)
    addi t1, sp, 16
    sd t1, 16(t0)

    sd ra, 8(t0)
    sd gp, 24(t0)
    sd tp, 32(t0)
    sd t2, 56(t0)
    sd s0, 64(t0)
    sd s1, 72(t0)
    sd a0, 80(t0)
    sd a1, 88(t0)
    sd a2, 96(t0)
    sd a3, 104(t0)
    sd a4, 112(t0)
    sd a5, 120(t0)
    sd a6, 128(t0)
    sd a7, 136(t0)
    sd s2, 144(t0)
    sd s3, 152(t0)
    sd s4, 160(t0)
    sd s5, 168(t0)
    sd s6, 176(t0)
    sd s7, 184(t0)
    sd s8, 192(t0)
    sd s9, 200(t0)
    sd s10, 208(t0)
    sd s11, 216(t0)
    sd t3, 224(t0)
    sd t4, 232(t0)
    sd t5, 240(t0)
    sd t6, 248(t0)

    csrr t1, sepc
    sd t1, 256(t0)
    csrr t1, sstatus
    sd t1, 264(t0)

    addi sp, sp, 16
    call onTrap

restore_context:
    la t1, currentCtx
    ld t1, 0(t1)

    ld ra, 8(t1)
    ld sp, 16(t1)
    ld gp, 24(t1)
    ld t2, 56(t1)
    ld s0, 64(t1)
    ld s1, 72(t1)
    ld a0, 80(t1)
    ld a1, 88(t1)
    ld a2, 96(t1)
    ld a3, 104(t1)
    ld a4, 112(t1)
    ld a5, 120(t1)
    ld a6, 128(t1)
    ld a7, 136(t1)
    ld s2, 144(t1)
    ld s3, 152(t1)
    ld s4, 160(t1)
    ld s5, 168(t1)
    ld s6, 176(t1)
    ld s7, 184(t1)
    ld s8, 192(t1)
    ld s9, 200(t1)
    ld s10, 208(t1)
    ld s11, 216(t1)
    ld t3, 224(t1)
    ld t4, 232(t1)
    ld t5, 240(t1)
    ld t6, 248(t1)

    ld t2, 264(t1)
    csrw sstatus, t2
    ld t2, 256(t1)
    csrw sepc, t2

    ld t0, 40(t1)
    ld t1, 48(t1)

    sret
