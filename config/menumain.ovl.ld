SECTIONS
{
    __romPos = 0;
    menumain_ROM_START = __romPos;
    menumain_VRAM = ADDR(.menumain);
    .menumain 0x801EF800 : AT(menumain_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menumain_TEXT_START = .;
        build/ovl/menumain/src/ovl/menumain/menumain.o(.text);
        menumain_TEXT_END = .;
        menumain_TEXT_SIZE = ABSOLUTE(menumain_TEXT_END - menumain_TEXT_START);
        menumain_RODATA_START = .;
        build/ovl/menumain/src/ovl/menumain/menumain.o(.rodata);
        build/ovl/menumain/asm/ovl/menumain/data/84E4.rodata.o(.rodata);
        menumain_RODATA_END = .;
        menumain_RODATA_SIZE = ABSOLUTE(menumain_RODATA_END - menumain_RODATA_START);
        menumain_DATA_START = .;
        build/ovl/menumain/src/ovl/menumain/menumain.o(.data);
        build/ovl/menumain/asm/ovl/menumain/data/8BB8.data.o(.data);
        build/ovl/menumain/src/ovl/menumain/menumain.o(.bss);
        menumain_DATA_END = .;
        menumain_DATA_SIZE = ABSOLUTE(menumain_DATA_END - menumain_DATA_START);
    }
    __romPos += SIZEOF(.menumain);
    __romPos = ALIGN(__romPos, 16);
    menumain_ROM_END = __romPos;
    menumain_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
