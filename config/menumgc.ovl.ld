SECTIONS
{
    __romPos = 0;
    menumgc_ROM_START = __romPos;
    menumgc_VRAM = ADDR(.menumgc);
    .menumgc 0x801E5800 : AT(menumgc_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menumgc_TEXT_START = .;
        build/ovl/menumgc/src/ovl/menumgc/menumgc.o(.text);
        menumgc_TEXT_END = .;
        menumgc_TEXT_SIZE = ABSOLUTE(menumgc_TEXT_END - menumgc_TEXT_START);
        menumgc_RODATA_START = .;
        build/ovl/menumgc/src/ovl/menumgc/menumgc.o(.rodata);
        build/ovl/menumgc/asm/ovl/menumgc/data/6E28.rodata.o(.rodata);
        build/ovl/menumgc/src/ovl/menumgc/menumgc.o(.bss);
        menumgc_RODATA_END = .;
        menumgc_RODATA_SIZE = ABSOLUTE(menumgc_RODATA_END - menumgc_RODATA_START);
        menumgc_DATA_START = .;
        build/ovl/menumgc/src/ovl/menumgc/menumgc.o(.data);
        menumgc_DATA_END = .;
        menumgc_DATA_SIZE = ABSOLUTE(menumgc_DATA_END - menumgc_DATA_START);
    }
    __romPos += SIZEOF(.menumgc);
    __romPos = ALIGN(__romPos, 16);
    menumgc_ROM_END = __romPos;
    menumgc_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
