#!/usr/bin/zsh

TMP_DIR="./tmp"
[ ! -z "$TMP_DIR" ] && GDB_TMP="$TMP_DIR""/gdb.tmp"

[ ! -d "$TMP_DIR" ] && mkdir "$TMP_DIR"

./bin/pE_application &
pid=$(ps -C pE_application -o pid=)
gdb --batch --command=examine_memory.gdb --pid="$pid" > "$GDB_TMP" && fg
