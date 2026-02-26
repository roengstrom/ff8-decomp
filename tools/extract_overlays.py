#!/usr/bin/env python3
"""Extract menu overlay files (.ovl) from FF8 Disc 1 (USA) BIN image.

FF8 stores all game data inside FF8DISC1.IMG on the disc. The IMG has a
file table at its start: pairs of (absolute_disc_sector, size_in_bytes).
The menu overlays are raw position-dependent MIPS binaries (no PS-X EXE
header) stored at IMG file table indices 4-20, matching the 17 overlay
descriptors in the main executable's table at D_80053C58.

Also extracts mngrp.bin (index 21) and init.out (index 22).
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

# IMG file table index -> overlay name (indices 4-22)
# Overlay table order matches D_80053C58 in the main executable.
IMG_FILES = {
    4: "menumain.ovl",
    5: "menucfg.ovl",
    6: "menupty.ovl",
    7: "menusts.ovl",
    8: "menuabl.ovl",
    9: "menushop.ovl",
    10: "menuext.ovl",
    11: "menuitem.ovl",
    12: "menumgc.ovl",
    13: "menugf.ovl",
    14: "menujnc2.ovl",
    15: "menusav.ovl",
    16: "menucrd.ovl",
    17: "menututo.ovl",
    18: "menutmag.ovl",
    19: "menutips.ovl",
    20: "menutest.ovl",
    21: "mngrp.bin",
    22: "init.out",
}

# Expected file name on disc
IMG_FILENAME = "FF8DISC1.IMG"


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


def extract_from_img(f, img_lba, mode2, output_dir):
    """Read the IMG file table and extract overlay files."""
    # The IMG file table is at the start of FF8DISC1.IMG.
    # Each entry is 8 bytes: (u32 absolute_disc_sector, u32 size_bytes).
    # We need enough entries to cover index 22 (init.out).
    table_sectors = 1  # file table fits in one sector
    table_data = read_sectors(f, img_lba, table_sectors, mode2)

    extracted = 0
    for idx in sorted(IMG_FILES.keys()):
        name = IMG_FILES[idx]
        sector, size = struct.unpack_from("<II", table_data, idx * 8)

        if sector == 0 or size == 0:
            print(f"  [{idx:2d}] {name}: empty entry, skipping")
            continue

        # Read file data sector by sector from the raw disc
        sector_count = (size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
        data = read_sectors(f, sector, sector_count, mode2)[:size]

        output_path = os.path.join(output_dir, name)
        with open(output_path, "wb") as out:
            out.write(data)

        size_str = f"{size / 1024:.1f} KB"
        if size >= 1024 * 1024:
            size_str = f"{size / 1024 / 1024:.1f} MB"
        print(f"  [{idx:2d}] {name:<16s} {size_str:>10s}  (sector {sector})")
        extracted += 1

    return extracted


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <disc.bin> [output_dir]")
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

        print(f"{IMG_FILENAME}: sector {img_lba}, {img_size / 1024 / 1024:.1f} MB")
        print()

        os.makedirs(output_dir, exist_ok=True)
        print(f"Extracting to {output_dir}/")
        count = extract_from_img(f, img_lba, mode2, output_dir)

    print()
    print(f"Done: {count} files extracted.")


if __name__ == "__main__":
    main()
