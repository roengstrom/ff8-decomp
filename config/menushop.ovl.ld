SECTIONS
{
    __romPos = 0;
    menushop_ROM_START = __romPos;
    menushop_VRAM = ADDR(.menushop);
    .menushop 0x801E5800 : AT(menushop_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menushop_TEXT_START = .;
        build/ovl/menushop/src/ovl/menushop/menushop.o(.text);
        menushop_TEXT_END = .;
        menushop_TEXT_SIZE = ABSOLUTE(menushop_TEXT_END - menushop_TEXT_START);
        menushop_RODATA_START = .;
        build/ovl/menushop/src/ovl/menushop/menushop.o(.rodata);
        build/ovl/menushop/asm/ovl/menushop/data/429C.rodata.o(.rodata);
        menushop_RODATA_END = .;
        menushop_RODATA_SIZE = ABSOLUTE(menushop_RODATA_END - menushop_RODATA_START);
        menushop_DATA_START = .;
        build/ovl/menushop/src/ovl/menushop/menushop.o(.data);
        build/ovl/menushop/asm/ovl/menushop/data/4570.data.o(.data);
        build/ovl/menushop/src/ovl/menushop/menushop.o(.bss);
        menushop_DATA_END = .;
        menushop_DATA_SIZE = ABSOLUTE(menushop_DATA_END - menushop_DATA_START);
    }
    __romPos += SIZEOF(.menushop);
    __romPos = ALIGN(__romPos, 16);
    menushop_ROM_END = __romPos;
    menushop_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
