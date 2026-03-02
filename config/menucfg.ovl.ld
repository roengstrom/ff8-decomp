SECTIONS
{
    __romPos = 0;
    menucfg_ROM_START = __romPos;
    menucfg_VRAM = ADDR(.menucfg);
    .menucfg 0x801E5800 : AT(menucfg_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menucfg_TEXT_START = .;
        build/ovl/menucfg/src/ovl/menucfg/menucfg.o(.text);
        menucfg_TEXT_END = .;
        menucfg_TEXT_SIZE = ABSOLUTE(menucfg_TEXT_END - menucfg_TEXT_START);
        menucfg_RODATA_START = .;
        build/ovl/menucfg/src/ovl/menucfg/menucfg.o(.rodata);
        build/ovl/menucfg/asm/ovl/menucfg/data/176C.rodata.o(.rodata);
        build/ovl/menucfg/src/ovl/menucfg/menucfg.o(.bss);
        menucfg_RODATA_END = .;
        menucfg_RODATA_SIZE = ABSOLUTE(menucfg_RODATA_END - menucfg_RODATA_START);
        menucfg_DATA_START = .;
        build/ovl/menucfg/src/ovl/menucfg/menucfg.o(.data);
        menucfg_DATA_END = .;
        menucfg_DATA_SIZE = ABSOLUTE(menucfg_DATA_END - menucfg_DATA_START);
    }
    __romPos += SIZEOF(.menucfg);
    __romPos = ALIGN(__romPos, 16);
    menucfg_ROM_END = __romPos;
    menucfg_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
