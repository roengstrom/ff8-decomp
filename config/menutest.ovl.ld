SECTIONS
{
    __romPos = 0;
    menutest_ROM_START = __romPos;
    menutest_VRAM = ADDR(.menutest);
    .menutest 0x801E5800 : AT(menutest_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        menutest_TEXT_START = .;
        build/ovl/menutest/src/ovl/menutest/menutest.o(.text);
        menutest_TEXT_END = .;
        menutest_TEXT_SIZE = ABSOLUTE(menutest_TEXT_END - menutest_TEXT_START);
        menutest_RODATA_START = .;
        build/ovl/menutest/src/ovl/menutest/menutest.o(.rodata);
        build/ovl/menutest/asm/ovl/menutest/data/1148.rodata.o(.rodata);
        build/ovl/menutest/src/ovl/menutest/menutest.o(.bss);
        menutest_RODATA_END = .;
        menutest_RODATA_SIZE = ABSOLUTE(menutest_RODATA_END - menutest_RODATA_START);
        menutest_DATA_START = .;
        build/ovl/menutest/src/ovl/menutest/menutest.o(.data);
        menutest_DATA_END = .;
        menutest_DATA_SIZE = ABSOLUTE(menutest_DATA_END - menutest_DATA_START);
    }
    __romPos += SIZEOF(.menutest);
    __romPos = ALIGN(__romPos, 16);
    menutest_ROM_END = __romPos;
    menutest_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
