#!/bin/bash
# Full rebuild: clean, split, extract assets, build, verify
set -e

echo "=== Clean ==="
make clean
rm -rf asm

echo "=== Split ==="
make split

echo "=== Build assets ==="
make build-assets

echo "=== Build & Verify ==="
make verify
