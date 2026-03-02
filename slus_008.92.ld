SECTIONS
{
    __romPos = 0;
    _gp = 0x8005EC24;
    header_ROM_START = __romPos;
    header_VRAM = ADDR(.header);
    .header : AT(header_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        header_DATA_START = .;
        build/asm/header.o(.data);
        header_DATA_END = .;
        header_DATA_SIZE = ABSOLUTE(header_DATA_END - header_DATA_START);
    }
    __romPos += SIZEOF(.header);
    header_ROM_END = __romPos;
    header_VRAM_END = .;

    main_ROM_START = __romPos;
    main_VRAM = ADDR(.main);
    .main 0x80010000 : AT(main_ROM_START) SUBALIGN(2)
    {
        FILL(0x00000000);
        main_RODATA_START = .;
        build/asm/data/800.rodata.o(.rodata);
        build/asm/data/9FC.rodata.o(.rodata);
        build/asm/data/F80.rodata.o(.rodata);
        build/asm/data/116C.rodata.o(.rodata);
        build/src/1D2C.o(.rodata);
        build/src/34C8.o(.rodata);
        build/src/10DD0.o(.rodata);
        build/src/psxsdk/libgcc.o(.rodata);
        build/src/1C2A0.o(.rodata);
        build/src/psxsdk/libsnd_reverb.o(.rodata);
        build/src/264E0.o(.rodata);
        build/src/psxsdk/softfloat.o(.rodata);
        build/src/psxsdk/libspu.o(.rodata);
        build/src/psxsdk/libgte.o(.rodata);
        build/src/psxsdk/libetc/pad_init.o(.rodata);
        build/src/psxsdk/libetc.o(.rodata);
        build/src/psxsdk/libcd.o(.rodata);
        build/src/psxsdk/libpress.o(.rodata);
        build/src/psxsdk/libapi.o(.rodata);
        build/src/psxsdk/libetc/pad.o(.rodata);
        build/src/psxsdk/libc.o(.rodata);
        build/src/psxsdk/libgpu.o(.rodata);
        build/src/psxsdk/libcard.o(.rodata);
        build/src/psxsdk/libsnd.o(.rodata);
        main_RODATA_END = .;
        main_RODATA_SIZE = ABSOLUTE(main_RODATA_END - main_RODATA_START);
        main_TEXT_START = .;
        build/src/1D2C.o(.text);
        build/src/34C8.o(.text);
        build/src/10DD0.o(.text);
        build/src/psxsdk/libgcc.o(.text);
        build/src/1C2A0.o(.text);
        build/src/psxsdk/libsnd_reverb.o(.text);
        build/src/264E0.o(.text);
        build/src/psxsdk/softfloat.o(.text);
        build/src/psxsdk/libspu.o(.text);
        build/src/psxsdk/libgte.o(.text);
        build/src/psxsdk/libetc/pad_init.o(.text);
        build/src/psxsdk/libetc.o(.text);
        build/src/psxsdk/libcd.o(.text);
        build/src/psxsdk/libpress.o(.text);
        build/src/psxsdk/libapi.o(.text);
        build/src/psxsdk/libetc/pad.o(.text);
        build/src/psxsdk/libc.o(.text);
        build/src/psxsdk/libgpu.o(.text);
        build/src/psxsdk/libcard.o(.text);
        build/src/psxsdk/libsnd.o(.text);
        main_TEXT_END = .;
        main_TEXT_SIZE = ABSOLUTE(main_TEXT_END - main_TEXT_START);
        main_DATA_START = .;
        build/asm/data/800.rodata.o(.data);
        build/asm/data/9FC.rodata.o(.data);
        build/asm/data/F80.rodata.o(.data);
        build/asm/data/116C.rodata.o(.data);
        build/src/1D2C.o(.data);
        build/src/34C8.o(.data);
        build/src/10DD0.o(.data);
        build/src/psxsdk/libgcc.o(.data);
        build/src/1C2A0.o(.data);
        build/src/psxsdk/libsnd_reverb.o(.data);
        build/src/264E0.o(.data);
        build/src/psxsdk/softfloat.o(.data);
        build/src/psxsdk/libspu.o(.data);
        build/src/psxsdk/libgte.o(.data);
        build/src/psxsdk/libetc/pad_init.o(.data);
        build/src/psxsdk/libetc.o(.data);
        build/src/psxsdk/libcd.o(.data);
        build/src/psxsdk/libpress.o(.data);
        build/src/psxsdk/libapi.o(.data);
        build/src/psxsdk/libetc/pad.o(.data);
        build/src/psxsdk/libc.o(.data);
        build/src/psxsdk/libgpu.o(.data);
        build/src/psxsdk/libcard.o(.data);
        build/src/psxsdk/libsnd.o(.data);
        build/asm/data/41EE8.data.o(.data);
        main_DATA_END = .;
        main_DATA_SIZE = ABSOLUTE(main_DATA_END - main_DATA_START);
        main_BSS_START = .;
        build/asm/data/800.rodata.o(.bss);
        build/asm/data/9FC.rodata.o(.bss);
        build/asm/data/F80.rodata.o(.bss);
        build/asm/data/116C.rodata.o(.bss);
        build/src/1D2C.o(.bss);
        build/src/34C8.o(.bss);
        build/src/10DD0.o(.bss);
        build/src/psxsdk/libgcc.o(.bss);
        build/src/1C2A0.o(.bss);
        build/src/psxsdk/libsnd_reverb.o(.bss);
        build/src/264E0.o(.bss);
        build/src/psxsdk/softfloat.o(.bss);
        build/src/psxsdk/libspu.o(.bss);
        build/src/psxsdk/libgte.o(.bss);
        build/src/psxsdk/libetc/pad_init.o(.bss);
        build/src/psxsdk/libetc.o(.bss);
        build/src/psxsdk/libcd.o(.bss);
        build/src/psxsdk/libpress.o(.bss);
        build/src/psxsdk/libapi.o(.bss);
        build/src/psxsdk/libetc/pad.o(.bss);
        build/src/psxsdk/libc.o(.bss);
        build/src/psxsdk/libgpu.o(.bss);
        build/src/psxsdk/libcard.o(.bss);
        build/src/psxsdk/libsnd.o(.bss);
        main_BSS_END = .;
        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);
    }
    __romPos += SIZEOF(.main);
    __romPos = ALIGN(__romPos, 16);
    main_ROM_END = __romPos;
    main_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
