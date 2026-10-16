#!/usr/bin/env python3
"""Makes the Bosman Edition logo: the original "Championship Manager Season 01/02" logo with a line
BOSMAN EDITION under SEASON 01/02, in the logo's own lettering. BOSMAN has the chrome of "Championship",
EDITION is italic in the yellow of "MANAGER" with its white rim and shadow (Max, 2026-10-16, Q211 to Q218).

Usage:
    python3 tools/logo/make_bosman_logo.py <original logo.png> <out.png> [<out-small.png>]

Neither picture goes into git (the logo rule). The site and the intro use the output
(site-release-assets/logo.png, tools/release/make_intro_logo.py). Needs Pillow.
"""
import os
import sys

def main():
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    src, out = sys.argv[1], sys.argv[2]
    if os.path.abspath(src) == os.path.abspath(out):
        sys.exit("the output must not overwrite the original logo")
    os.environ['BOSMAN_LOGO_SRC'] = src
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import style
    w, h = style.compose(out)
    if len(sys.argv) == 4:
        from PIL import Image
        Image.open(out).resize((w // 2, h // 2), Image.LANCZOS).save(sys.argv[3])
    print(f"{out}: {w}x{h}")

if __name__ == '__main__':
    main()
