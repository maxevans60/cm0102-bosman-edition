"""Unpack the database files of Football Manager 2007 (db/700) from the disc's setup.jar, as data.

Usage: python3 -I unpack_setup_jar.py <setup.jar> <db700 folder>

Mount the disc image read-only first (macOS: hdiutil attach -readonly -nobrowse <image>). setup.jar is an
InstallShield MultiPlatform archive (a zip). Nothing from it runs: this script only reads zip entries and zlib
data. The files are untrusted input, so every size is capped, every output name is a plain file name, and every
file must match the MD5 that the archive's index gives for it.

Layout:
- install/data/<hash>/7.0.0/assembly.dat is itself a zip. Its entry <hash>/index is a run of zlib blocks.
- The unpacked index lists each installed path as: 16 bytes MD5, u16 big-endian length, the path, then the size.
- Each file is the zip entry .../md5/<md5 in hex> of setup.jar, zlib-compressed.

Output: the files directly under db/700/ (client_db.dat, server_db.dat, people_db.dat and the history files).
"""
import hashlib
import io
import os
import re
import sys
import zipfile
import zlib

MAX_FILE = 512 * 1024 * 1024     # no db/700 file is near this; a larger one stops the script
MAX_INDEX = 64 * 1024 * 1024


def need(cond, msg):
    if not cond:
        sys.exit("unpack_setup_jar: " + msg)


def read_entry(z, info, most):
    need(info.file_size <= most, f"{info.filename} is larger than {most} bytes")
    with z.open(info) as f:
        data = f.read(most + 1)
    need(len(data) <= most, f"{info.filename} is larger than {most} bytes")
    return data


def inflate(data, most):
    o = zlib.decompressobj()
    out = o.decompress(data, most + 1)
    need(len(out) <= most and not o.unconsumed_tail, f"zlib data larger than {most} bytes")
    return out, o.unused_data


def unindex(d):
    """The zlib blocks of the index, joined (bytes between blocks are skipped)."""
    out, i = b"", 0
    while i < len(d):
        j = d.find(b"\x78\x9c", i)
        if j < 0:
            break
        try:
            r, rest = inflate(d[j:], MAX_INDEX - len(out))
            out += r
            i = len(d) - len(rest)
        except zlib.error:
            i = j + 1
    return out


def main(argv):
    need(len(argv) == 2, "usage: unpack_setup_jar.py <setup.jar> <db700 folder>")
    jar, out = argv
    os.makedirs(out, exist_ok=True)
    z = zipfile.ZipFile(jar)
    asm = [i for i in z.infolist() if re.fullmatch(r"install/data/[0-9a-f]{32}/7\.0\.0/assembly\.dat", i.filename)]
    need(len(asm) == 1, "assembly.dat not found once in setup.jar")
    a = zipfile.ZipFile(io.BytesIO(read_entry(z, asm[0], MAX_INDEX)))
    idx = [i for i in a.infolist() if re.fullmatch(r"[0-9a-f]{32}/index", i.filename)]
    need(len(idx) == 1, "index not found once in assembly.dat")
    raw = unindex(read_entry(a, idx[0], MAX_INDEX))
    md5 = {i.filename.rsplit("/", 1)[1]: i for i in z.infolist() if "/md5/" in i.filename}
    n = 0
    for m in re.finditer(rb"db/700/[A-Za-z0-9_.-]+", raw):
        name, s = m.group().decode(), m.start()
        if s < 18 or int.from_bytes(raw[s - 2:s], "big") != len(name):
            continue
        h = raw[s - 18:s - 2].hex()
        info = md5.get(h)
        if info is None or not info.file_size:
            continue
        data = read_entry(z, info, MAX_FILE)
        try:
            data, _ = inflate(data, MAX_FILE)
        except zlib.error:
            pass
        need(hashlib.md5(data).hexdigest() == h, f"{name}: MD5 does not match the index")
        base = os.path.basename(name)
        need(base and base not in (".", "..") and "/" not in base, f"bad file name {name!r}")
        with open(os.path.join(out, base), "wb") as f:
            f.write(data)
        print(f"{base} {len(data)} bytes, MD5 ok")
        n += 1
    need(n, "no db/700 files found")


if __name__ == "__main__":
    main(sys.argv[1:])
