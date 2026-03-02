SECTIONS
{
    __romPos = 0;
    menujnc2_ROM_START = __romPos;
    menujnc2_VRAM = ADDR(.menujnc2);
    .menujnc2 0x801E5800 : AT(menujnc2_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menujnc2_TEXT_START = .;
        build/ovl/menujnc2/src/ovl/menujnc2/menujnc2.o(.text);
        menujnc2_TEXT_END = .;
        menujnc2_TEXT_SIZE = ABSOLUTE(menujnc2_TEXT_END - menujnc2_TEXT_START);
        menujnc2_RODATA_START = .;
        build/ovl/menujnc2/src/ovl/menujnc2/menujnc2.o(.rodata);
        build/ovl/menujnc2/asm/ovl/menujnc2/data/90C4.rodata.o(.rodata);
        build/ovl/menujnc2/src/ovl/menujnc2/menujnc2.o(.bss);
        menujnc2_RODATA_END = .;
        menujnc2_RODATA_SIZE = ABSOLUTE(menujnc2_RODATA_END - menujnc2_RODATA_START);
        menujnc2_DATA_START = .;
        build/ovl/menujnc2/src/ovl/menujnc2/menujnc2.o(.data);
        menujnc2_DATA_END = .;
        menujnc2_DATA_SIZE = ABSOLUTE(menujnc2_DATA_END - menujnc2_DATA_START);
    }
    __romPos += SIZEOF(.menujnc2);
    __romPos = ALIGN(__romPos, 16);
    menujnc2_ROM_END = __romPos;
    menujnc2_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
