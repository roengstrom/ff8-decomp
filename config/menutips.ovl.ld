SECTIONS
{
    __romPos = 0;
    menutips_ROM_START = __romPos;
    menutips_VRAM = ADDR(.menutips);
    .menutips 0x801E5800 : AT(menutips_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menutips_TEXT_START = .;
        build/ovl/menutips/src/ovl/menutips/menutips.o(.text);
        menutips_TEXT_END = .;
        menutips_TEXT_SIZE = ABSOLUTE(menutips_TEXT_END - menutips_TEXT_START);
        menutips_RODATA_START = .;
        build/ovl/menutips/src/ovl/menutips/menutips.o(.rodata);
        build/ovl/menutips/asm/ovl/menutips/data/1268.rodata.o(.rodata);
        build/ovl/menutips/src/ovl/menutips/menutips.o(.bss);
        menutips_RODATA_END = .;
        menutips_RODATA_SIZE = ABSOLUTE(menutips_RODATA_END - menutips_RODATA_START);
        menutips_DATA_START = .;
        build/ovl/menutips/src/ovl/menutips/menutips.o(.data);
        menutips_DATA_END = .;
        menutips_DATA_SIZE = ABSOLUTE(menutips_DATA_END - menutips_DATA_START);
    }
    __romPos += SIZEOF(.menutips);
    __romPos = ALIGN(__romPos, 16);
    menutips_ROM_END = __romPos;
    menutips_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
