#!/usr/bin/zsh

TMP_DIR="./tmp"
[ ! -z "$TMP_DIR" ] && GDB_TMP="$TMP_DIR""/gdb.tmp"

[ ! -d "$TMP_DIR" ] && mkdir "$TMP_DIR"

./bin/pE_application &
pid=$(ps -C "pE_application" -o pid=)
echo "$pid"
gdb --batch  --command=examine_memory.gdb --pid="$pid" 2>&1
#pE_jid=$(jobs | grep pE_application | grep -oE "^\[[0-9]\]" | sed 's/\[//g' | sed 's/\]//g')
#fg
