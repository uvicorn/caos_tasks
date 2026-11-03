#!/bin/bash

for cmd in gdb cmake ltrace strace; do
    if command -v "$cmd" &>/dev/null; then
        echo "$cmd: OK"
    else
        echo "$cmd: NOT FOUND"
    fi
done

wget -O ~/.gdbinit-gef.py -q https://gef.blah.cat/py

cat > file.txt <<'EOF'
source ~/.gdbinit-gef.py
alias ir  = info registers
alias afl =info functions
set disassembly-flavor intel
set history save
set history filename ~/.gdb_history
set verbose off
set print pretty on
set print array off
set print array-indexes on
set python print-stack full
EOF