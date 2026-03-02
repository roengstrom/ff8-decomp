SECTIONS
{
    __romPos = 0;
    menucrd_ROM_START = __romPos;
    menucrd_VRAM = ADDR(.menucrd);
    .menucrd 0x801E5800 : AT(menucrd_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menucrd_TEXT_START = .;
        build/ovl/menucrd/src/ovl/menucrd/menucrd.o(.text);
        menucrd_TEXT_END = .;
        menucrd_TEXT_SIZE = ABSOLUTE(menucrd_TEXT_END - menucrd_TEXT_START);
        menucrd_RODATA_START = .;
        build/ovl/menucrd/src/ovl/menucrd/menucrd.o(.rodata);
        build/ovl/menucrd/asm/ovl/menucrd/data/14A8.rodata.o(.rodata);
        menucrd_RODATA_END = .;
        menucrd_RODATA_SIZE = ABSOLUTE(menucrd_RODATA_END - menucrd_RODATA_START);
        menucrd_DATA_START = .;
        build/ovl/menucrd/src/ovl/menucrd/menucrd.o(.data);
        build/ovl/menucrd/asm/ovl/menucrd/data/2164.data.o(.data);
        build/ovl/menucrd/src/ovl/menucrd/menucrd.o(.bss);
        menucrd_DATA_END = .;
        menucrd_DATA_SIZE = ABSOLUTE(menucrd_DATA_END - menucrd_DATA_START);
    }
    __romPos += SIZEOF(.menucrd);
    __romPos = ALIGN(__romPos, 16);
    menucrd_ROM_END = __romPos;
    menucrd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
