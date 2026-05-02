# S-mode entry point, called by OpenSBI
# a0 = hartid, a1 = DTB address

.section .text
.global _entry
_entry:
    mv tp, a0                  # store hartid in tp for cpuid()
    la sp, stack0
    li t0, 1024*4
    addi a0, a0, 1
    mul t0, t0, a0
    add sp, sp, t0
    call start
spin:
    j spin
