#!/bin/sh

CMD="$1"
OUTPUT="perf-${CMD}-$(date +"%Y%m%d-%H:%M").svg"

perf record --call-graph dwarf "./$CMD"
perf script | /opt/FlameGraph/stackcollapse-perf.pl | /opt/FlameGraph/flamegraph.pl > "$OUTPUT"
chromium "file://$PWD/$OUTPUT" &

