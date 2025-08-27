ENTRY(_start)

SECTIONS {
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot_header)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }

    /* Ensure page tables are page-aligned */
    . = ALIGN(4K);
    p4_table = .;
    . += 4K;
    p3_table = .;
    . += 4K;
    p2_table = .;
    . += 4K;
}