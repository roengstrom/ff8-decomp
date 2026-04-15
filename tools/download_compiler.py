#!/usr/bin/env python3
"""Download a PS1 compiler binary from decomp.me's ghcr.io compilers image.

Usage:
    python3 tools/download_compiler.py <compiler_id> <dest_dir>

Example:
    python3 tools/download_compiler.py gcc2.7.2-cdk tools/gcc-2.7.2-cdk
    python3 tools/download_compiler.py gcc2.8.0-psx tools/gcc-2.8.0-psx
"""
import json
import os
import sys
import tarfile
import tempfile
import urllib.request


def get(url, headers=None, stream=False):
    req = urllib.request.Request(url, headers=headers or {})
    return urllib.request.urlopen(req, timeout=60)


def fetch_json(url, headers=None):
    return json.loads(get(url, headers).read().decode())


def main():
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit(1)

    compiler_id = sys.argv[1]
    dest_dir = sys.argv[2]
    image = f"ps1/{compiler_id}"

    # Get anonymous pull token for this image
    token_url = f"https://ghcr.io/token?scope=repository:decompme/compilers/{image}:pull"
    token = fetch_json(token_url)["token"]
    auth = {"Authorization": f"Bearer {token}"}

    # Fetch multi-arch index and find the amd64 manifest
    idx = fetch_json(
        f"https://ghcr.io/v2/decompme/compilers/{image}/manifests/latest",
        {**auth, "Accept": "application/vnd.oci.image.index.v1+json"},
    )
    amd64_digest = next(
        m["digest"]
        for m in idx["manifests"]
        if m["platform"]["architecture"] == "amd64"
    )

    # Fetch the amd64 manifest and get its single tarball layer
    mani = fetch_json(
        f"https://ghcr.io/v2/decompme/compilers/{image}/manifests/{amd64_digest}",
        {**auth, "Accept": "application/vnd.oci.image.manifest.v1+json"},
    )
    layer_digest = mani["layers"][0]["digest"]

    # Download the layer tarball
    print(f"Downloading {compiler_id}...")
    with tempfile.NamedTemporaryFile(suffix=".tar.gz", delete=False) as tmp:
        resp = get(
            f"https://ghcr.io/v2/decompme/compilers/{image}/blobs/{layer_digest}",
            auth,
        )
        while chunk := resp.read(1 << 16):
            tmp.write(chunk)
        tmp_path = tmp.name

    # Extract — the tarball has `compilers/ps1/<compiler_id>/...` layout; we want the
    # contents of that directory flattened into dest_dir. We only need the compiler
    # binary and gcc driver; skip bundled maspsx (we have our own), docs, tests, etc.
    os.makedirs(dest_dir, exist_ok=True)
    prefix = f"compilers/ps1/{compiler_id}/"
    wanted = {"cc1", "cc1plus", "cpp", "gcc", "g++"}
    with tarfile.open(tmp_path, "r:gz") as tar:
        for member in tar.getmembers():
            if not member.name.startswith(prefix):
                continue
            rel = member.name[len(prefix):]
            # Only install top-level binaries we care about
            if "/" in rel or rel not in wanted:
                continue
            if not member.isfile():
                continue
            out = os.path.join(dest_dir, rel)
            with open(out, "wb") as f:
                src = tar.extractfile(member)
                if src is not None:
                    f.write(src.read())
            if member.mode & 0o111:
                os.chmod(out, 0o755)

    os.unlink(tmp_path)
    print(f"  Installed to {dest_dir}/")


if __name__ == "__main__":
    main()
