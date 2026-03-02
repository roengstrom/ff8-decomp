SECTIONS
{
    __romPos = 0;
    menupty_ROM_START = __romPos;
    menupty_VRAM = ADDR(.menupty);
    .menupty 0x801E5800 : AT(menupty_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menupty_TEXT_START = .;
        build/ovl/menupty/src/ovl/menupty/menupty.o(.text);
        menupty_TEXT_END = .;
        menupty_TEXT_SIZE = ABSOLUTE(menupty_TEXT_END - menupty_TEXT_START);
        menupty_RODATA_START = .;
        build/ovl/menupty/src/ovl/menupty/menupty.o(.rodata);
        build/ovl/menupty/asm/ovl/menupty/data/3C7C.rodata.o(.rodata);
        build/ovl/menupty/src/ovl/menupty/menupty.o(.bss);
        menupty_RODATA_END = .;
        menupty_RODATA_SIZE = ABSOLUTE(menupty_RODATA_END - menupty_RODATA_START);
        menupty_DATA_START = .;
        build/ovl/menupty/src/ovl/menupty/menupty.o(.data);
        menupty_DATA_END = .;
        menupty_DATA_SIZE = ABSOLUTE(menupty_DATA_END - menupty_DATA_START);
    }
    __romPos += SIZEOF(.menupty);
    __romPos = ALIGN(__romPos, 16);
    menupty_ROM_END = __romPos;
    menupty_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
