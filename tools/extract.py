#!/usr/bin/env python3
"""Extract SLUS_008.92 + all overlays from FF8 Disc 1 (USA) BIN image.

Verifies the disc image SHA1 against the Redump-verified hash before
extracting, so a wrong/modified disc is caught upfront.

Usage:
    python3 tools/extract.py <disc.bin> [output_dir]

The output directory defaults to ./original/ (matching the Makefile).
Extracts:
  - SLUS_008.92              (game executable, SHA1-verified)
  - field_init.bin           (init stub)
  - display_init.bin         (LZSS-compressed)
  - field_engine.bin         (LZSS-compressed)
  - battle_engine.bin        (LZSS-compressed)
  - battle_render.bin
  - battle_code.bin
  - world_engine.bin     (LZSS-compressed)
  - menumain.ovl .. menutest.ovl  (17 menu overlays)
  - mngrp.bin, init.out      (data files)
"""

import hashlib
import os
import struct
import sys

# PS1 CD-ROM raw sector layout (2352 bytes per sector)
# Mode 1: 12 sync + 4 header + 2048 data + 4 EDC + 276 ECC
# Mode 2: 12 sync + 4 header + 8 subheader + 2048 data + 4 EDC + 276 ECC
SECTOR_SIZE_RAW = 2352
SECTOR_DATA_OFFSET_MODE1 = 16
SECTOR_DATA_OFFSET_MODE2 = 24
SECTOR_DATA_SIZE = 2048

# ISO 9660: PVD is at sector 16
PVD_SECTOR = 16
PVD_TYPE = 1
PVD_MAGIC = b"CD001"

# FF8 game data container inside the ISO.
IMG_FILENAME = "FF8DISC1.IMG"

# Expected hashes (Redump-verified FF8 USA Disc 1, SLUS-00892).
EXPECTED_DISC_SHA1 = "5028a4f815a50d6ffe16f9c3e44ec6235a81f3f5"
EXPECTED_DISC_SIZE = 732_271_680  # bytes
EXPECTED_EXE_NAME = "SLUS_008.92"
EXPECTED_EXE_SHA1 = "40706b4e0553fc6cbeb044ca1e0e9004d5ac2561"

# Files to extract from FF8DISC1.IMG's internal file table.
# Each entry: (IMG table index, output filename, compression).
#   "raw"  = copy bytes directly
#   "lzss" = LZSS-decompress before writing (matches func_80039520)
IMG_FILES = [
    # Code overlays (all load to 0x80098000)
    (0,  "field_init.bin",       "raw"),
    (1,  "display_init.bin",     "lzss"),
    (2,  "field_engine.bin",     "lzss"),
    # Menu overlays (load to 0x801E5800 / 0x801EF800 / ...)
    (4,  "menumain.ovl",         "raw"),
    (5,  "menucfg.ovl",          "raw"),
    (6,  "menupty.ovl",          "raw"),
    (7,  "menusts.ovl",          "raw"),
    (8,  "menuabl.ovl",          "raw"),
    (9,  "menushop.ovl",         "raw"),
    (10, "menuext.ovl",          "raw"),
    (11, "menuitem.ovl",         "raw"),
    (12, "menumgc.ovl",          "raw"),
    (13, "menugf.ovl",           "raw"),
    (14, "menujnc2.ovl",         "raw"),
    (15, "menusav.ovl",          "raw"),
    (16, "menucrd.ovl",          "raw"),
    (17, "menututo.ovl",         "raw"),
    (18, "menutmag.ovl",         "raw"),
    (19, "menutips.ovl",         "raw"),
    (20, "menutest.ovl",         "raw"),
    # Data files
    (21, "mngrp.bin",            "raw"),
    (22, "init.out",             "raw"),
    # Battle overlays (load to 0x80098000)
    (23, "battle_engine.bin",    "lzss"),
    (24, "battle_render.bin",    "raw"),
    (25, "battle_code.bin",      "raw"),
    (26, "world_engine.bin", "lzss"),
]


# ---------------------------------------------------------------------------
# Sector / ISO 9660 reading
# ---------------------------------------------------------------------------

def read_sector(f, sector_num, mode2=True):
    """Read the 2048-byte data portion of a raw CD sector."""
    f.seek(sector_num * SECTOR_SIZE_RAW)
    raw = f.read(SECTOR_SIZE_RAW)
    if len(raw) < SECTOR_SIZE_RAW:
        raise ValueError(f"Short read at sector {sector_num}")
    offset = SECTOR_DATA_OFFSET_MODE2 if mode2 else SECTOR_DATA_OFFSET_MODE1
    return raw[offset : offset + SECTOR_DATA_SIZE]


def read_sectors(f, start, count, mode2=True):
    """Read N consecutive sectors and return concatenated user data."""
    return b"".join(read_sector(f, start + i, mode2) for i in range(count))


def detect_mode(f):
    """Auto-detect Mode 1 vs Mode 2 sectors by locating the PVD."""
    for mode2 in (True, False):
        pvd = read_sector(f, PVD_SECTOR, mode2=mode2)
        if pvd[0] == PVD_TYPE and pvd[1:6] == PVD_MAGIC:
            return pvd, mode2
    return None, None


def find_in_root(f, pvd, mode2, target_name):
    """Search the ISO 9660 root directory for target_name. Returns (lba, size)."""
    root_lba = struct.unpack_from("<I", pvd, 156 + 2)[0]
    root_size = struct.unpack_from("<I", pvd, 156 + 10)[0]
    sector_count = (root_size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
    dir_data = read_sectors(f, root_lba, sector_count, mode2)

    offset = 0
    while offset < root_size:
        record_len = dir_data[offset]
        if record_len == 0:
            # Advance to the next sector boundary
            offset = ((offset // SECTOR_DATA_SIZE) + 1) * SECTOR_DATA_SIZE
            continue
        lba = struct.unpack_from("<I", dir_data, offset + 2)[0]
        size = struct.unpack_from("<I", dir_data, offset + 10)[0]
        name_len = dir_data[offset + 32]
        name = dir_data[offset + 33 : offset + 33 + name_len].decode("ascii", errors="replace")
        if ";" in name:
            name = name[: name.index(";")]
        if name == target_name:
            return lba, size
        offset += record_len
    return None, None


# ---------------------------------------------------------------------------
# LZSS decompression (matches func_80039520 in the game binary)
# ---------------------------------------------------------------------------

def lzss_decompress(compressed):
    """LZSS decompression with 4096-byte sliding window.

    Format:
      - 4-byte header: token count (decremented per token consumed)
      - Flag byte: 8 bits, each controlling one token
        - bit=1: literal (copy 1 byte from source)
        - bit=0: back-reference (2 bytes → 12-bit offset + 4-bit length,
                  copies length+3 bytes from sliding window)
      - Sliding window offset bias -0xFEE
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
            if src >= len(compressed):
                break
            dst.append(compressed[src])
            src += 1
            token_count -= 1
        else:
            if src + 1 >= len(compressed):
                break
            byte1, byte2 = compressed[src], compressed[src + 1]
            src += 2
            offset_val = byte1 | ((byte2 & 0xF0) << 4)
            length = (byte2 & 0x0F) + 3
            adjusted = offset_val - 0xFEE
            src_offset = (len(dst) - adjusted) & 0xFFF
            src_ptr = len(dst) - src_offset
            for _ in range(length):
                dst.append(dst[src_ptr] if src_ptr >= 0 else 0)
                src_ptr += 1
            token_count -= 2

        flags >>= 1
        flag_bits -= 1

    return bytes(dst)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def sha1_file(path, chunk_size=1 << 20):
    """Compute SHA1 of a file, streaming in chunks."""
    h = hashlib.sha1()
    size = os.path.getsize(path)
    read = 0
    with open(path, "rb") as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            h.update(chunk)
            read += len(chunk)
            pct = 100 * read / size
            sys.stderr.write(f"\r  Hashing disc image... {pct:5.1f}%")
            sys.stderr.flush()
    sys.stderr.write("\r" + " " * 40 + "\r")
    return h.hexdigest()


def format_size(size):
    return f"{size / 1024 / 1024:.1f} MB" if size >= 1024 * 1024 else f"{size / 1024:.1f} KB"


def verify_disc(disc_path):
    """Verify the disc image against Redump's hash. Aborts on mismatch."""
    size = os.path.getsize(disc_path)
    if size != EXPECTED_DISC_SIZE:
        print(f"Error: disc image size {size:,} bytes, expected {EXPECTED_DISC_SIZE:,}")
        print("       This does not look like FF8 USA Disc 1 (SLUS-00892).")
        sys.exit(1)

    actual = sha1_file(disc_path)
    if actual != EXPECTED_DISC_SHA1:
        print(f"Error: disc SHA1 mismatch")
        print(f"       actual:   {actual}")
        print(f"       expected: {EXPECTED_DISC_SHA1}")
        print("       Use a Redump-verified FF8 USA Disc 1 BIN image.")
        sys.exit(1)


def extract_exe(f, mode2, pvd, output_dir):
    """Extract and verify SLUS_008.92 from the disc root."""
    lba, size = find_in_root(f, pvd, mode2, EXPECTED_EXE_NAME)
    if lba is None:
        print(f"Error: {EXPECTED_EXE_NAME} not found in root directory")
        sys.exit(1)

    sector_count = (size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
    data = read_sectors(f, lba, sector_count, mode2)[:size]

    out_path = os.path.join(output_dir, EXPECTED_EXE_NAME)
    with open(out_path, "wb") as out:
        out.write(data)

    actual = hashlib.sha1(data).hexdigest()
    if actual != EXPECTED_EXE_SHA1:
        print(f"Error: extracted {EXPECTED_EXE_NAME} SHA1 mismatch")
        print(f"       actual:   {actual}")
        print(f"       expected: {EXPECTED_EXE_SHA1}")
        sys.exit(1)

    print(f"  {EXPECTED_EXE_NAME:<24s} {format_size(size):>10s}  (sector {lba}, SHA1 ✓)")


def extract_overlays(f, mode2, pvd, output_dir):
    """Extract all IMG_FILES from FF8DISC1.IMG."""
    img_lba, img_size = find_in_root(f, pvd, mode2, IMG_FILENAME)
    if img_lba is None:
        print(f"Error: {IMG_FILENAME} not found in root directory")
        sys.exit(1)

    # IMG file table is at the start of FF8DISC1.IMG:
    # pairs of (u32 absolute_disc_sector, u32 size_bytes).
    table_data = read_sectors(f, img_lba, 1, mode2)

    for idx, name, compression in IMG_FILES:
        sector, size = struct.unpack_from("<II", table_data, idx * 8)
        if sector == 0 or size == 0:
            print(f"  [{idx:2d}] {name}: empty entry, skipping")
            continue

        sector_count = (size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
        raw = read_sectors(f, sector, sector_count, mode2)[:size]

        if compression == "lzss":
            data = lzss_decompress(raw)
            tag = "LZSS"
            size_str = f"{format_size(size):>10s} → {format_size(len(data)):>10s}"
        else:
            data = raw
            tag = " raw"
            size_str = f"{format_size(size):>10s}              "

        with open(os.path.join(output_dir, name), "wb") as out:
            out.write(data)
        print(f"  [{idx:2d}] {name:<24s} [{tag}] {size_str}  (sector {sector})")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    disc_path = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) >= 3 else os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "original"
    )

    if not os.path.isfile(disc_path):
        print(f"Error: disc image not found: {disc_path}")
        sys.exit(1)

    print(f"Disc: {disc_path}")
    print(f"Output: {output_dir}/")
    print()

    print("Verifying disc image...")
    verify_disc(disc_path)
    print(f"  SHA1 ✓ {EXPECTED_DISC_SHA1}")
    print()

    os.makedirs(output_dir, exist_ok=True)

    with open(disc_path, "rb") as f:
        pvd, mode2 = detect_mode(f)
        if pvd is None:
            print("Error: could not find ISO 9660 PVD")
            sys.exit(1)

        print("Extracting executable...")
        extract_exe(f, mode2, pvd, output_dir)
        print()

        print("Extracting overlays from FF8DISC1.IMG...")
        extract_overlays(f, mode2, pvd, output_dir)

    print()
    print("Done.")


if __name__ == "__main__":
    main()
