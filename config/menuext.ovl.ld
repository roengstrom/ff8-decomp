SECTIONS
{
    __romPos = 0;
    menuext_ROM_START = __romPos;
    menuext_VRAM = ADDR(.menuext);
    .menuext 0x801E5800 : AT(menuext_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menuext_TEXT_START = .;
        build/ovl/menuext/src/ovl/menuext/menuext.o(.text);
        menuext_TEXT_END = .;
        menuext_TEXT_SIZE = ABSOLUTE(menuext_TEXT_END - menuext_TEXT_START);
        menuext_RODATA_START = .;
        build/ovl/menuext/src/ovl/menuext/menuext.o(.rodata);
        build/ovl/menuext/asm/ovl/menuext/data/3220.rodata.o(.rodata);
        build/ovl/menuext/src/ovl/menuext/menuext.o(.bss);
        menuext_RODATA_END = .;
        menuext_RODATA_SIZE = ABSOLUTE(menuext_RODATA_END - menuext_RODATA_START);
        menuext_DATA_START = .;
        build/ovl/menuext/src/ovl/menuext/menuext.o(.data);
        menuext_DATA_END = .;
        menuext_DATA_SIZE = ABSOLUTE(menuext_DATA_END - menuext_DATA_START);
    }
    __romPos += SIZEOF(.menuext);
    __romPos = ALIGN(__romPos, 16);
    menuext_ROM_END = __romPos;
    menuext_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
