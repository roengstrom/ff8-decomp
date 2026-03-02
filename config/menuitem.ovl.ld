SECTIONS
{
    __romPos = 0;
    menuitem_ROM_START = __romPos;
    menuitem_VRAM = ADDR(.menuitem);
    .menuitem 0x801E2800 : AT(menuitem_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menuitem_TEXT_START = .;
        build/ovl/menuitem/src/ovl/menuitem/menuitem.o(.text);
        menuitem_TEXT_END = .;
        menuitem_TEXT_SIZE = ABSOLUTE(menuitem_TEXT_END - menuitem_TEXT_START);
        menuitem_RODATA_START = .;
        build/ovl/menuitem/src/ovl/menuitem/menuitem.o(.rodata);
        build/ovl/menuitem/asm/ovl/menuitem/data/8654.rodata.o(.rodata);
        menuitem_RODATA_END = .;
        menuitem_RODATA_SIZE = ABSOLUTE(menuitem_RODATA_END - menuitem_RODATA_START);
        menuitem_DATA_START = .;
        build/ovl/menuitem/src/ovl/menuitem/menuitem.o(.data);
        build/ovl/menuitem/asm/ovl/menuitem/data/9F1C.data.o(.data);
        build/ovl/menuitem/src/ovl/menuitem/menuitem.o(.bss);
        menuitem_DATA_END = .;
        menuitem_DATA_SIZE = ABSOLUTE(menuitem_DATA_END - menuitem_DATA_START);
    }
    __romPos += SIZEOF(.menuitem);
    __romPos = ALIGN(__romPos, 16);
    menuitem_ROM_END = __romPos;
    menuitem_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
