SECTIONS
{
    __romPos = 0;
    menugf_ROM_START = __romPos;
    menugf_VRAM = ADDR(.menugf);
    .menugf 0x801E5800 : AT(menugf_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menugf_TEXT_START = .;
        build/ovl/menugf/src/ovl/menugf/menugf.o(.text);
        menugf_TEXT_END = .;
        menugf_TEXT_SIZE = ABSOLUTE(menugf_TEXT_END - menugf_TEXT_START);
        menugf_RODATA_START = .;
        build/ovl/menugf/src/ovl/menugf/menugf.o(.rodata);
        build/ovl/menugf/asm/ovl/menugf/data/2550.rodata.o(.rodata);
        build/ovl/menugf/src/ovl/menugf/menugf.o(.bss);
        menugf_RODATA_END = .;
        menugf_RODATA_SIZE = ABSOLUTE(menugf_RODATA_END - menugf_RODATA_START);
        menugf_DATA_START = .;
        build/ovl/menugf/src/ovl/menugf/menugf.o(.data);
        menugf_DATA_END = .;
        menugf_DATA_SIZE = ABSOLUTE(menugf_DATA_END - menugf_DATA_START);
    }
    __romPos += SIZEOF(.menugf);
    __romPos = ALIGN(__romPos, 16);
    menugf_ROM_END = __romPos;
    menugf_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
