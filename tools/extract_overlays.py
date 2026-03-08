#!/usr/bin/env python3
"""Extract game files from FF8 Disc 1 (USA) BIN image.

FF8 stores all game data inside FF8DISC1.IMG on the disc. The IMG has a
file table at its start: pairs of (absolute_disc_sector, size_in_bytes).

Extracts:
  - Code overlays (entries 0-2, 23-26): dynamic code loaded to 0x80098000.
    Entries 1, 2, 23, 26 are LZSS-compressed on disc and decompressed here.
  - Menu overlays (entries 4-20): raw MIPS binaries for 17 menu screens.
  - Data files (entries 21-22): mngrp.bin and init.out.
"""

import os
import struct
import sys

# PS1 CD-ROM raw sector layout (2352 bytes per sector)
SECTOR_SIZE_RAW = 2352
SECTOR_DATA_OFFSET_MODE1 = 16
SECTOR_DATA_OFFSET_MODE2 = 24
SECTOR_DATA_SIZE = 2048

# ISO 9660
PVD_SECTOR = 16
PVD_TYPE = 1
PVD_MAGIC = b"CD001"

# Expected file name on disc
IMG_FILENAME = "FF8DISC1.IMG"

# IMG file table entries to extract.
# Format: index -> (output_name, compression_type)
#   "raw"  = copy bytes directly from disc
#   "lzss" = LZSS-decompress before writing (matches func_80039520)
IMG_FILES = {
    # Code overlays (load to 0x80098000)
    0: ("field_init.bin", "raw"),        # Init stub (2,450 B)
    1: ("display_init.bin", "lzss"),     # Display init (2,530 → 4,784 B)
    2: ("field_engine.bin", "lzss"),     # Field engine (102,523 → 191,872 B)
    # Menu overlays (load to 0x801Exxxx)
    4: ("menumain.ovl", "raw"),
    5: ("menucfg.ovl", "raw"),
    6: ("menupty.ovl", "raw"),
    7: ("menusts.ovl", "raw"),
    8: ("menuabl.ovl", "raw"),
    9: ("menushop.ovl", "raw"),
    10: ("menuext.ovl", "raw"),
    11: ("menuitem.ovl", "raw"),
    12: ("menumgc.ovl", "raw"),
    13: ("menugf.ovl", "raw"),
    14: ("menujnc2.ovl", "raw"),
    15: ("menusav.ovl", "raw"),
    16: ("menucrd.ovl", "raw"),
    17: ("menututo.ovl", "raw"),
    18: ("menutmag.ovl", "raw"),
    19: ("menutips.ovl", "raw"),
    20: ("menutest.ovl", "raw"),
    # Data files
    21: ("mngrp.bin", "raw"),
    22: ("init.out", "raw"),
    # Battle overlays (load to 0x80098000)
    23: ("battle_engine.bin", "lzss"),   # Battle engine+data (683,741 → 1,092,672 B)
    24: ("battle_render.bin", "raw"),    # Battle render (80,376 B)
    25: ("battle_code.bin", "raw"),      # Battle code (347,208 B)
    26: ("field_engine_alt.bin", "lzss"),  # Alt field engine (118,482 → 198,197 B)
}


def read_sector(f, sector_num, mode2=True):
    """Read 2048 bytes of user data from a raw CD sector."""
    f.seek(sector_num * SECTOR_SIZE_RAW)
    raw = f.read(SECTOR_SIZE_RAW)
    if len(raw) < SECTOR_SIZE_RAW:
        raise ValueError(f"Short read at sector {sector_num}")
    offset = SECTOR_DATA_OFFSET_MODE2 if mode2 else SECTOR_DATA_OFFSET_MODE1
    return raw[offset : offset + SECTOR_DATA_SIZE]


def read_sectors(f, start, count, mode2=True):
    """Read consecutive sectors, return concatenated user data."""
    data = bytearray()
    for i in range(count):
        data.extend(read_sector(f, start + i, mode2))
    return bytes(data)


def detect_mode(f):
    """Detect sector mode by finding the PVD."""
    for mode2 in (True, False):
        pvd = read_sector(f, PVD_SECTOR, mode2=mode2)
        if pvd[0] == PVD_TYPE and pvd[1:6] == PVD_MAGIC:
            return pvd, mode2
    return None, None


def find_img(f, pvd, mode2):
    """Find FF8DISC1.IMG in the ISO root directory. Returns (lba, size)."""
    root_lba = struct.unpack_from("<I", pvd, 156 + 2)[0]
    root_size = struct.unpack_from("<I", pvd, 156 + 10)[0]
    sector_count = (root_size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
    dir_data = read_sectors(f, root_lba, sector_count, mode2)

    offset = 0
    while offset < root_size:
        record_len = dir_data[offset]
        if record_len == 0:
            next_sector = ((offset // SECTOR_DATA_SIZE) + 1) * SECTOR_DATA_SIZE
            if next_sector >= root_size:
                break
            offset = next_sector
            continue

        lba = struct.unpack_from("<I", dir_data, offset + 2)[0]
        size = struct.unpack_from("<I", dir_data, offset + 10)[0]
        name_len = dir_data[offset + 32]
        name_raw = dir_data[offset + 33 : offset + 33 + name_len]

        if name_len > 1:
            name = name_raw.decode("ascii", errors="replace")
            if ";" in name:
                name = name[: name.index(";")]
            if name == IMG_FILENAME:
                return lba, size

        offset += record_len

    return None, None


def lzss_decompress(compressed):
    """LZSS decompression matching func_80039520 in the game binary.

    Algorithm:
      - 4-byte header: token count (decremented per token consumed)
      - Flag byte: 8 bits, each controlling one token
        - bit=1: literal (copy 1 byte from source)
        - bit=0: back-reference (2 bytes → 12-bit offset + 4-bit length,
                  copies length+3 bytes from sliding window)
      - 4096-byte sliding window with offset bias -0xFEE (-4078)
      - Token count decrements by 1 per literal/flag, 2 per back-reference
    """
    if len(compressed) < 4:
        raise ValueError("Compressed data too short (< 4 bytes)")

    token_count = struct.unpack_from("<I", compressed, 0)[0]
    src = 4
    dst = bytearray()
    flags = 0
    flag_bits = 0

    while token_count > 0:
        if flag_bits == 0:
            if src >= len(compressed):
                break
            flags = compressed[src]
            src += 1
            token_count -= 1
            if token_count <= 0:
                break
            flag_bits = 8

        if flags & 1:
            # Literal byte
            if src >= len(compressed):
                break
            dst.append(compressed[src])
            src += 1
            token_count -= 1
        else:
            # Back-reference: 2 bytes encode offset (12 bits) + length (4 bits)
            if src + 1 >= len(compressed):
                break
            byte1 = compressed[src]
            byte2 = compressed[src + 1]
            src += 2

            offset_val = byte1 | ((byte2 & 0xF0) << 4)
            length = (byte2 & 0x0F) + 3

            # Sliding window position
            adjusted = offset_val - 0xFEE
            current_pos = len(dst)
            src_offset = (current_pos - adjusted) & 0xFFF
            src_ptr = len(dst) - src_offset

            for _ in range(length):
                if src_ptr < 0:
                    dst.append(0)
                else:
                    dst.append(dst[src_ptr])
                src_ptr += 1

            token_count -= 2

        flags >>= 1
        flag_bits -= 1

    return bytes(dst)


def format_size(size):
    """Format byte size as human-readable string."""
    if size >= 1024 * 1024:
        return f"{size / 1024 / 1024:.1f} MB"
    return f"{size / 1024:.1f} KB"


def extract_from_img(f, img_lba, mode2, output_dir):
    """Read the IMG file table and extract files."""
    # The IMG file table is at the start of FF8DISC1.IMG.
    # Each entry is 8 bytes: (u32 absolute_disc_sector, u32 size_bytes).
    # Need enough sectors to cover all entries (max index 26 → 27 entries × 8 = 216 bytes).
    table_data = read_sectors(f, img_lba, 1, mode2)

    extracted = 0
    for idx in sorted(IMG_FILES.keys()):
        name, compression = IMG_FILES[idx]
        sector, size = struct.unpack_from("<II", table_data, idx * 8)

        if sector == 0 or size == 0:
            print(f"  [{idx:2d}] {name}: empty entry, skipping")
            continue

        # Read raw data from disc
        sector_count = (size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
        raw_data = read_sectors(f, sector, sector_count, mode2)[:size]

        if compression == "lzss":
            data = lzss_decompress(raw_data)
            size_str = (
                f"{format_size(size):>10s} → {format_size(len(data)):>10s}"
            )
        else:
            data = raw_data
            size_str = f"{format_size(size):>10s}              "

        output_path = os.path.join(output_dir, name)
        with open(output_path, "wb") as out:
            out.write(data)

        tag = "LZSS" if compression == "lzss" else " raw"
        print(f"  [{idx:2d}] {name:<24s} [{tag}] {size_str}  (sector {sector})")
        extracted += 1

    return extracted


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <disc.bin> [output_dir]")
        print()
        print("Extract game files from FF8 Disc 1 (USA) raw BIN image.")
        print("LZSS-compressed overlays are decompressed automatically.")
        sys.exit(1)

    disc_path = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) >= 3 else os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "original"
    )

    if not os.path.isfile(disc_path):
        print(f"Error: disc image not found: {disc_path}")
        sys.exit(1)

    with open(disc_path, "rb") as f:
        pvd, mode2 = detect_mode(f)
        if pvd is None:
            print("Error: could not find ISO 9660 PVD")
            sys.exit(1)

        img_lba, img_size = find_img(f, pvd, mode2)
        if img_lba is None:
            print(f"Error: {IMG_FILENAME} not found on disc")
            sys.exit(1)

        print(f"{IMG_FILENAME}: sector {img_lba}, {format_size(img_size)}")
        print()

        os.makedirs(output_dir, exist_ok=True)
        print(f"Extracting to {output_dir}/")
        count = extract_from_img(f, img_lba, mode2, output_dir)

    print()
    print(f"Done: {count} files extracted.")


if __name__ == "__main__":
    main()
