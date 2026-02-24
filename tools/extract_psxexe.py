#!/usr/bin/env python3
"""Extract PS-EXE (SLUS_008.92) from FF8 Disc 1 (USA) BIN image."""

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

EXPECTED_EXE = "SLUS_008.92"
EXPECTED_SHA1 = "40706b4e0553fc6cbeb044ca1e0e9004d5ac2561"


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


def parse_directory_record(data, offset):
    """Parse an ISO 9660 directory record. Returns (name, lba, size, record_len) or None."""
    record_len = data[offset]
    if record_len == 0:
        return None
    lba = struct.unpack_from("<I", data, offset + 2)[0]
    size = struct.unpack_from("<I", data, offset + 10)[0]
    name_len = data[offset + 32]
    name_raw = data[offset + 33 : offset + 33 + name_len]
    if name_len == 1 and name_raw[0] in (0, 1):
        name = "." if name_raw[0] == 0 else ".."
    else:
        name = name_raw.decode("ascii", errors="replace")
        if ";" in name:
            name = name[: name.index(";")]
    return (name, lba, size, record_len)


def list_root_files(f, root_lba, root_size, mode2):
    """List all files in the root directory."""
    sector_count = (root_size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
    dir_data = read_sectors(f, root_lba, sector_count, mode2)
    entries = []
    offset = 0
    while offset < root_size:
        rec = parse_directory_record(dir_data, offset)
        if rec is None:
            next_sector = ((offset // SECTOR_DATA_SIZE) + 1) * SECTOR_DATA_SIZE
            if next_sector >= root_size:
                break
            offset = next_sector
            continue
        entries.append(rec[:3])  # (name, lba, size)
        offset += rec[3]
    return entries


def extract_file(f, lba, size, output_path, mode2):
    """Extract a file from the disc image."""
    sector_count = (size + SECTOR_DATA_SIZE - 1) // SECTOR_DATA_SIZE
    data = read_sectors(f, lba, sector_count, mode2)[:size]
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, "wb") as out:
        out.write(data)
    return data


def detect_mode(f):
    """Detect sector mode by finding the PVD."""
    for mode2 in (True, False):
        pvd = read_sector(f, PVD_SECTOR, mode2=mode2)
        if pvd[0] == PVD_TYPE and pvd[1:6] == PVD_MAGIC:
            return pvd, mode2
    return None, None


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

        root_lba = struct.unpack_from("<I", pvd, 156 + 2)[0]
        root_size = struct.unpack_from("<I", pvd, 156 + 10)[0]
        entries = list_root_files(f, root_lba, root_size, mode2)

        # Find and extract the PS-EXE
        for name, lba, size in entries:
            if name == EXPECTED_EXE:
                output_path = os.path.join(output_dir, name)
                print(f"Extracting {name} ({size:,} bytes) -> {output_path}")
                data = extract_file(f, lba, size, output_path, mode2)

                sha1 = hashlib.sha1(data).hexdigest()
                print(f"  SHA1: {sha1}")
                if sha1 == EXPECTED_SHA1:
                    print("  SHA1 matches expected value.")
                else:
                    print(f"  WARNING: expected SHA1 {EXPECTED_SHA1}")
                    sys.exit(1)
                return

        print(f"Error: {EXPECTED_EXE} not found on disc")
        print("Files found:", ", ".join(name for name, _, _ in entries))
        sys.exit(1)


if __name__ == "__main__":
    main()
