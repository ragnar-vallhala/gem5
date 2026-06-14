#!/usr/bin/env bash
# M1 capability study: sweep external-memory wait states on CoreMark and report
# CPI / cycle count. Uses the dataWaitStates CPU param (run_coremark.py reads
# AVR_DATA_WS). simInsts is constant across the sweep (same instruction stream);
# only the cycle count grows with wait states.
set -euo pipefail
GEM5=build/AVR/gem5.opt
printf "%-4s %14s %12s %7s %9s\n" WS numCycles simInsts CPI slowdown
base=""
for ws in 0 1 2 3 4; do
    out="m5out_ws$ws"
    AVR_DATA_WS=$ws "$GEM5" --outdir="$out" run_coremark.py >/dev/null 2>&1
    nc=$(awk '/system.cpu.numCycles /{print $2; exit}' "$out/stats.txt")
    si=$(awk '/^simInsts /{print $2; exit}' "$out/stats.txt")
    cpi=$(awk "BEGIN{printf \"%.3f\", $nc/$si}")
    [ -z "$base" ] && base=$nc
    sd=$(awk "BEGIN{printf \"%.3fx\", $nc/$base}")
    printf "%-4s %14s %12s %7s %9s\n" "$ws" "$nc" "$si" "$cpi" "$sd"
done
