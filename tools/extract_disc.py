#!/usr/bin/env python3
"""Extract files from a PS1 BIN/CUE disc image (MODE2/2352)."""

import os
import struct
import sys

# MODE2/2352: each raw sector is 2352 bytes
# Layout: 12 sync + 4 header + 8 subheader + 2048 data + 280 EDC/ECC
RAW_SECTOR_SIZE = 2352
SYNC_SIZE = 12
HEADER_SIZE = 4
SUBHEADER_SIZE = 8
DATA_OFFSET = SYNC_SIZE + HEADER_SIZE + SUBHEADER_SIZE  # 24
DATA_SIZE = 2048


def read_sector(f, sector_num):
    """Read the 2048-byte data portion of a raw sector."""
    f.seek(sector_num * RAW_SECTOR_SIZE + DATA_OFFSET)
    return f.read(DATA_SIZE)


def read_sectors(f, sector_num, count):
    """Read data from multiple consecutive sectors."""
    data = bytearray()
    for i in range(count):
        data.extend(read_sector(f, sector_num + i))
    return bytes(data)


def parse_directory_record(data, offset):
    """Parse a single ISO 9660 directory record."""
    if offset >= len(data):
        return None, 0
    length = data[offset]
    if length == 0:
        return None, 0
    if offset + length > len(data):
        return None, 0

    extent = struct.unpack_from('<I', data, offset + 2)[0]
    size = struct.unpack_from('<I', data, offset + 10)[0]
    flags = data[offset + 25]
    name_length = data[offset + 32]
    name_bytes = data[offset + 33: offset + 33 + name_length]

    try:
        name = name_bytes.decode('ascii', errors='replace')
    except:
        name = name_bytes.hex()

    is_dir = bool(flags & 0x02)

    # Strip version number (;1)
    if ';' in name:
        name = name[:name.index(';')]
    # Remove trailing dot if present
    if name.endswith('.'):
        name = name[:-1]

    return {
        'name': name,
        'extent': extent,
        'size': size,
        'is_dir': is_dir,
        'flags': flags,
    }, length


def read_directory(f, extent, size):
    """Read all directory records from a directory extent."""
    sector_count = (size + DATA_SIZE - 1) // DATA_SIZE
    data = read_sectors(f, extent, sector_count)

    entries = []
    offset = 0
    while offset < size:
        record, length = parse_directory_record(data, offset)
        if record is None or length == 0:
            # Skip to next sector boundary
            next_boundary = ((offset // DATA_SIZE) + 1) * DATA_SIZE
            if next_boundary <= offset:
                break
            offset = next_boundary
            continue
        entries.append(record)
        offset += length
    return entries


def extract_directory(f, extent, size, output_dir, depth=0):
    """Recursively extract a directory from the disc image."""
    entries = read_directory(f, extent, size)

    for entry in entries:
        name = entry['name']
        # Skip current and parent directory entries
        if name == '\x00' or name == '\x01' or name == '' or name == '\x00' * len(name):
            continue
        if len(name) == 1 and ord(name[0]) <= 1:
            continue

        indent = "  " * depth
        target_path = os.path.join(output_dir, name)

        if entry['is_dir']:
            print(f"{indent}[DIR]  {name}/")
            os.makedirs(target_path, exist_ok=True)
            extract_directory(f, entry['extent'], entry['size'], target_path, depth + 1)
        else:
            size_kb = entry['size'] / 1024
            if size_kb >= 1024:
                size_str = f"{size_kb/1024:.1f} MB"
            else:
                size_str = f"{size_kb:.1f} KB"
            print(f"{indent}[FILE] {name} ({size_str})")

            # Extract file
            remaining = entry['size']
            sector = entry['extent']
            with open(target_path, 'wb') as out:
                while remaining > 0:
                    sector_data = read_sector(f, sector)
                    chunk = sector_data[:min(remaining, DATA_SIZE)]
                    out.write(chunk)
                    remaining -= len(chunk)
                    sector += 1


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <disc.bin> [output_dir]")
        sys.exit(1)

    bin_path = sys.argv[1]
    output_dir = sys.argv[2] if len(sys.argv) >= 3 else os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "disc"
    )

    if not os.path.exists(bin_path):
        print(f"Error: {bin_path} not found")
        sys.exit(1)

    os.makedirs(output_dir, exist_ok=True)

    print(f"Opening: {bin_path}")
    print(f"Size: {os.path.getsize(bin_path) / 1024 / 1024:.1f} MB")
    print(f"Output: {output_dir}/")
    print()

    with open(bin_path, 'rb') as f:
        # Read Primary Volume Descriptor (sector 16)
        pvd = read_sector(f, 16)

        if pvd[0] != 1 or pvd[1:6] != b'CD001':
            print("Error: Primary Volume Descriptor not found at sector 16")
            for s in [0, 16, 17, 18]:
                test = read_sector(f, s)
                print(f"  Sector {s}: type={test[0]}, id={test[1:6]}")
            sys.exit(1)

        volume_id = pvd[40:72].decode('ascii', errors='replace').strip()
        print(f"Volume: {volume_id}")

        # Root directory record is at offset 156 in the PVD, 34 bytes long
        root_extent = struct.unpack_from('<I', pvd, 156 + 2)[0]
        root_size = struct.unpack_from('<I', pvd, 156 + 10)[0]

        print(f"Root directory: sector {root_extent}, size {root_size}")
        print()
        print("Extracting files...")
        print("=" * 50)

        extract_directory(f, root_extent, root_size, output_dir)

    print()
    print("=" * 50)
    print("Extraction complete!")

    # Summary
    total_files = 0
    total_dirs = 0
    total_size = 0
    for root, dirs, files in os.walk(output_dir):
        total_dirs += len(dirs)
        total_files += len(files)
        for fname in files:
            total_size += os.path.getsize(os.path.join(root, fname))

    print(f"  Directories: {total_dirs}")
    print(f"  Files: {total_files}")
    print(f"  Total size: {total_size / 1024 / 1024:.1f} MB")


if __name__ == '__main__':
    main()
