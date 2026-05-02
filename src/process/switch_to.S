.section .text
.global switch_to
switch_to:
    # a0 = ProcessContext* nextCtx
    la t0, currentCtx
    sd a0, 0(t0)          # currentCtx = nextCtx
    j restore_context     # jump to trap return path in TrapEntry.S
