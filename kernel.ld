OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS
{
    . = 0x80000000;
    .text : ALIGN(0x1000)
    {
        *(.text)
        . = ALIGN(0x4);
        onTrap = .;
    }

    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    .rodata : ALIGN(0x1000)
    {
        *(.rodata)
    }

    .bss : ALIGN(0x1000)
    {
        *(COMMON)
        *(.bss)
    }

    PROVIDE(KERNEL_END_ADDRESS = .);
}