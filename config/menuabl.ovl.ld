SECTIONS
{
    __romPos = 0;
    menuabl_ROM_START = __romPos;
    menuabl_VRAM = ADDR(.menuabl);
    .menuabl 0x801E2800 : AT(menuabl_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menuabl_TEXT_START = .;
        build/ovl/menuabl/src/ovl/menuabl/menuabl.o(.text);
        menuabl_TEXT_END = .;
        menuabl_TEXT_SIZE = ABSOLUTE(menuabl_TEXT_END - menuabl_TEXT_START);
        menuabl_RODATA_START = .;
        build/ovl/menuabl/src/ovl/menuabl/menuabl.o(.rodata);
        build/ovl/menuabl/asm/ovl/menuabl/data/1508.rodata.o(.rodata);
        build/ovl/menuabl/src/ovl/menuabl/menuabl.o(.bss);
        menuabl_RODATA_END = .;
        menuabl_RODATA_SIZE = ABSOLUTE(menuabl_RODATA_END - menuabl_RODATA_START);
        menuabl_DATA_START = .;
        build/ovl/menuabl/src/ovl/menuabl/menuabl.o(.data);
        menuabl_DATA_END = .;
        menuabl_DATA_SIZE = ABSOLUTE(menuabl_DATA_END - menuabl_DATA_START);
    }
    __romPos += SIZEOF(.menuabl);
    __romPos = ALIGN(__romPos, 16);
    menuabl_ROM_END = __romPos;
    menuabl_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
