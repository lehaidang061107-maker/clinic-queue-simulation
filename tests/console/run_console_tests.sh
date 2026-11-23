#!/bin/sh
# Chay cac kich ban menu trong tests/console va so voi tep .expected.
# Dung tu thu muc goc: sh tests/console/run_console_tests.sh [mau_ten]
# Moi dong trong <ca>.expected phai xuat hien (chuoi con) trong dau ra theo dung thu tu.
# Neu kich ban luu ket qua vao build/console/<ca>_results.txt thi tep do duoc noi vao dau ra.
# data/patients_working.txt duoc tam cat di truoc moi ca va tra lai sau cung,
# de moi ca bat dau tu kho rong (yeu cau "ca tu chua" cua giai doan 3).
set -u
BIN=./build/clinic_simulator
[ -x "$BIN" ] || BIN=./build/clinic_simulator.exe
[ -x "$BIN" ] || { echo "Chua build: chay make truoc."; exit 2; }
OUT=build/console
mkdir -p "$OUT"
WORKING=data/patients_working.txt
BACKUP="$OUT/patients_working.backup"
rm -f "$BACKUP"
[ -f "$WORKING" ] && mv "$WORKING" "$BACKUP"
restore() { rm -f "$WORKING"; [ -f "$BACKUP" ] && mv "$BACKUP" "$WORKING"; }
trap restore EXIT INT TERM

pattern=${1:-}
pass=0; fail=0
for input in tests/console/*${pattern}*.in; do
    [ -f "$input" ] || continue
    name=$(basename "$input" .in)
    expected="tests/console/$name.expected"
    rm -f "$WORKING" "$OUT/${name}_results.txt" "$OUT/${name}_working.txt"
    "$BIN" < "$input" > "$OUT/$name.out" 2>&1
    status=$?
    [ -f "$OUT/${name}_results.txt" ] && cat "$OUT/${name}_results.txt" >> "$OUT/$name.out"
    if [ ! -f "$expected" ]; then
        echo "SKIP $name (khong co .expected, exit=$status)"; continue
    fi
    # Kiem tra tung dong mong doi xuat hien theo thu tu.
    if awk -v expfile="$expected" '
        BEGIN { n = 0; while ((getline line < expfile) > 0) { sub(/\r$/, "", line); if (line != "") want[++n] = line } i = 1 }
        { sub(/\r$/, ""); while (i <= n && index($0, want[i]) > 0) i++ }
        END { if (i <= n) { print "  thieu: " want[i]; exit 1 } }' "$OUT/$name.out" && [ "$status" -eq 0 ]; then
        echo "PASS $name"; pass=$((pass + 1))
    else
        echo "FAIL $name (exit=$status, dau ra: $OUT/$name.out)"; fail=$((fail + 1))
    fi
done
echo "Console: $pass dat, $fail chua dat."
[ "$fail" -eq 0 ]
