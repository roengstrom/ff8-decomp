SECTIONS
{
    __romPos = 0;
    menusav_ROM_START = __romPos;
    menusav_VRAM = ADDR(.menusav);
    .menusav 0x801E2800 : AT(menusav_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menusav_TEXT_START = .;
        build/ovl/menusav/src/ovl/menusav/menusav.o(.text);
        menusav_TEXT_END = .;
        menusav_TEXT_SIZE = ABSOLUTE(menusav_TEXT_END - menusav_TEXT_START);
        menusav_RODATA_START = .;
        build/ovl/menusav/src/ovl/menusav/menusav.o(.rodata);
        build/ovl/menusav/asm/ovl/menusav/data/91E0.rodata.o(.rodata);
        build/ovl/menusav/src/ovl/menusav/menusav.o(.bss);
        menusav_RODATA_END = .;
        menusav_RODATA_SIZE = ABSOLUTE(menusav_RODATA_END - menusav_RODATA_START);
        menusav_DATA_START = .;
        build/ovl/menusav/src/ovl/menusav/menusav.o(.data);
        menusav_DATA_END = .;
        menusav_DATA_SIZE = ABSOLUTE(menusav_DATA_END - menusav_DATA_START);
    }
    __romPos += SIZEOF(.menusav);
    __romPos = ALIGN(__romPos, 16);
    menusav_ROM_END = __romPos;
    menusav_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
