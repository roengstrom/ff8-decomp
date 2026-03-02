SECTIONS
{
    __romPos = 0;
    menututo_ROM_START = __romPos;
    menututo_VRAM = ADDR(.menututo);
    .menututo 0x801E2800 : AT(menututo_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menututo_TEXT_START = .;
        build/ovl/menututo/src/ovl/menututo/menututo.o(.text);
        menututo_TEXT_END = .;
        menututo_TEXT_SIZE = ABSOLUTE(menututo_TEXT_END - menututo_TEXT_START);
        menututo_RODATA_START = .;
        build/ovl/menututo/src/ovl/menututo/menututo.o(.rodata);
        build/ovl/menututo/asm/ovl/menututo/data/2538.rodata.o(.rodata);
        build/ovl/menututo/src/ovl/menututo/menututo.o(.bss);
        menututo_RODATA_END = .;
        menututo_RODATA_SIZE = ABSOLUTE(menututo_RODATA_END - menututo_RODATA_START);
        menututo_DATA_START = .;
        build/ovl/menututo/src/ovl/menututo/menututo.o(.data);
        menututo_DATA_END = .;
        menututo_DATA_SIZE = ABSOLUTE(menututo_DATA_END - menututo_DATA_START);
    }
    __romPos += SIZEOF(.menututo);
    __romPos = ALIGN(__romPos, 16);
    menututo_ROM_END = __romPos;
    menututo_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
