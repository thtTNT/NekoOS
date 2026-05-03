.section .text
.global switch_to
switch_to:
    # a0 = ProcessContext* nextCtx, a1 = trapSP
    mv sp, a1                 # sp ← trapSP（restore_context 的 csrw sscratch, sp 需要）
    la t0, currentCtx
    sd a0, 0(t0)              # currentCtx = nextCtx
    j restore_context         # jump to trap return path in TrapEntry.S
