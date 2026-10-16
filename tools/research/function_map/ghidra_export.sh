#!/bin/sh
# Decompiles every game and library function of the original with Ghidra headless, once.
# Writes $FUNCMAP_WORK/ghidra/out/<address>.c. Needs Ghidra and a Java 21 runtime.
# Usage: CM0102_EXE=... FUNCMAP_WORK=... ghidra_export.sh
set -e
: "${CM0102_EXE:?set CM0102_EXE}" "${FUNCMAP_WORK:?set FUNCMAP_WORK}"
HERE=$(cd "$(dirname "$0")" && pwd)
GHIDRA=${GHIDRA_HEADLESS:-/opt/homebrew/Cellar/ghidra/12.1.4/libexec/support/analyzeHeadless}
export JAVA_HOME=${JAVA_HOME:-/opt/homebrew/opt/openjdk@21}
mkdir -p "$FUNCMAP_WORK/ghidra/proj" "$FUNCMAP_WORK/ghidra/out"
awk -F'\t' 'NR > 1 && ($2 == "game" || $2 == "lib") { sub(/^0x/, "", $1); print $1 }' \
    "$HERE/../../../docs/research/function-map/function-files.tsv" > "$FUNCMAP_WORK/ghidra/functions.txt"
"$GHIDRA" "$FUNCMAP_WORK/ghidra/proj" cm0102 -import "$CM0102_EXE" -overwrite -scriptPath "$HERE" \
    -postScript ExportDecomp.java "$FUNCMAP_WORK/ghidra/functions.txt" "$FUNCMAP_WORK/ghidra/out" -max-cpu 6
