SECTIONS
{
    __romPos = 0;
    menusts_ROM_START = __romPos;
    menusts_VRAM = ADDR(.menusts);
    .menusts 0x801E5800 : AT(menusts_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menusts_TEXT_START = .;
        build/ovl/menusts/src/ovl/menusts/menusts.o(.text);
        menusts_TEXT_END = .;
        menusts_TEXT_SIZE = ABSOLUTE(menusts_TEXT_END - menusts_TEXT_START);
        menusts_RODATA_START = .;
        build/ovl/menusts/src/ovl/menusts/menusts.o(.rodata);
        build/ovl/menusts/asm/ovl/menusts/data/3CC8.rodata.o(.rodata);
        menusts_RODATA_END = .;
        menusts_RODATA_SIZE = ABSOLUTE(menusts_RODATA_END - menusts_RODATA_START);
        menusts_DATA_START = .;
        build/ovl/menusts/src/ovl/menusts/menusts.o(.data);
        build/ovl/menusts/asm/ovl/menusts/data/42E4.data.o(.data);
        build/ovl/menusts/src/ovl/menusts/menusts.o(.bss);
        menusts_DATA_END = .;
        menusts_DATA_SIZE = ABSOLUTE(menusts_DATA_END - menusts_DATA_START);
    }
    __romPos += SIZEOF(.menusts);
    __romPos = ALIGN(__romPos, 16);
    menusts_ROM_END = __romPos;
    menusts_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
