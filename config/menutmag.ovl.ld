SECTIONS
{
    __romPos = 0;
    menutmag_ROM_START = __romPos;
    menutmag_VRAM = ADDR(.menutmag);
    .menutmag 0x801E5800 : AT(menutmag_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menutmag_TEXT_START = .;
        build/ovl/menutmag/src/ovl/menutmag/menutmag.o(.text);
        menutmag_TEXT_END = .;
        menutmag_TEXT_SIZE = ABSOLUTE(menutmag_TEXT_END - menutmag_TEXT_START);
        menutmag_RODATA_START = .;
        build/ovl/menutmag/src/ovl/menutmag/menutmag.o(.rodata);
        build/ovl/menutmag/asm/ovl/menutmag/data/BA8.rodata.o(.rodata);
        build/ovl/menutmag/src/ovl/menutmag/menutmag.o(.bss);
        menutmag_RODATA_END = .;
        menutmag_RODATA_SIZE = ABSOLUTE(menutmag_RODATA_END - menutmag_RODATA_START);
        menutmag_DATA_START = .;
        build/ovl/menutmag/src/ovl/menutmag/menutmag.o(.data);
        menutmag_DATA_END = .;
        menutmag_DATA_SIZE = ABSOLUTE(menutmag_DATA_END - menutmag_DATA_START);
    }
    __romPos += SIZEOF(.menutmag);
    __romPos = ALIGN(__romPos, 16);
    menutmag_ROM_END = __romPos;
    menutmag_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
