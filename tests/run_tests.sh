#!/usr/bin/env bash
# =============================================================
#  run_tests.sh — Compiler batch test suite
#  Usage (from WSL):
#    cd /mnt/d/Projects/Psudo_code_to_c/tests
#    bash run_tests.sh
# =============================================================

COMPILER="/mnt/d/Projects/Psudo_code_to_c/logic/my_compiler"
TESTS_DIR="/mnt/d/Projects/Psudo_code_to_c/tests"

PASS=0; FAIL=0; WARN=0
FAILED_TESTS=()

# Colours
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

sep() { printf "${CYAN}────────────────────────────────────────────────────────${RESET}\n"; }

# run_test <id> <description> <input_file>
#           <expect_exit: 0|1>   (0=success, 1=compile fail)
#           <expect_stdout_has>  (substring; empty = dont check)
#           <expect_stdout_not>  (substring that must NOT appear; empty = skip)
#           <expect_stderr_has>  (substring; empty = no stderr expected)
run_test() {
    local id="$1" desc="$2" file="$3"
    local expect_exit="$4"
    local expect_out_has="$5"
    local expect_out_not="$6"
    local expect_err_has="$7"

    local stdout stderr exit_code

    stdout=$("$COMPILER" < "$file" 2>/tmp/_test_stderr)
    exit_code=$?
    stderr=$(cat /tmp/_test_stderr)

    local failures=()

    # 1. Exit code
    if [[ "$expect_exit" != "" ]] && [[ "$exit_code" != "$expect_exit" ]]; then
        failures+=("Exit code: expected $expect_exit, got $exit_code")
    fi

    # 2. stdout must contain
    if [[ -n "$expect_out_has" ]] && ! grep -qF "$expect_out_has" <<< "$stdout"; then
        failures+=("stdout missing: '$expect_out_has'")
    fi

    # 3. stdout must NOT contain
    if [[ -n "$expect_out_not" ]] && grep -qF "$expect_out_not" <<< "$stdout"; then
        failures+=("stdout should NOT contain: '$expect_out_not'")
    fi

    # 4. stderr must contain
    if [[ -n "$expect_err_has" ]] && ! grep -qF "$expect_err_has" <<< "$stderr"; then
        failures+=("stderr missing: '$expect_err_has'")
    fi

    # 5. Unexpected stderr when none expected
    if [[ -z "$expect_err_has" ]] && [[ -n "$stderr" ]]; then
        WARN=$((WARN + 1))
        printf "  ${YELLOW}⚠  WARN${RESET}  [%s] %s\n" "$id" "$desc"
        printf "       stderr: %s\n" "$stderr"
        return
    fi

    if [[ ${#failures[@]} -eq 0 ]]; then
        PASS=$((PASS + 1))
        printf "  ${GREEN}✔  PASS${RESET}  [%s] %s\n" "$id" "$desc"
    else
        FAIL=$((FAIL + 1))
        FAILED_TESTS+=("[$id] $desc")
        printf "  ${RED}✖  FAIL${RESET}  [%s] %s\n" "$id" "$desc"
        for f in "${failures[@]}"; do
            printf "       ${RED}→ %s${RESET}\n" "$f"
        done
        if [[ -n "$stdout" ]]; then
            printf "       stdout:\n%s\n" "$stdout" | head -20 | sed 's/^/         /'
        fi
        if [[ -n "$stderr" ]]; then
            printf "       stderr: %s\n" "$stderr"
        fi
    fi
}

# =============================================================
printf "\n${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}\n"
printf "${BOLD}  Pseudo → C  Compiler  Test Suite${RESET}\n"
printf "${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}\n\n"

# ─── CATEGORY 1: VALID PROGRAMS ───────────────────────────────
sep
printf "${BOLD}  Category 1: Valid Programs (clean output, no stderr)${RESET}\n"
sep

run_test "01" "Hello World (SHOW string)" \
    "$TESTS_DIR/01_hello_world.pseudo" \
    0 'printf("%s' '' ''

run_test "02" "Variable declaration + assign + SHOW var" \
    "$TESTS_DIR/02_var_and_assign.pseudo" \
    0 'int x;' '' ''

run_test "03" "Arithmetic: +  -  *" \
    "$TESTS_DIR/03_arithmetic.pseudo" \
    0 'int a;' '' ''

run_test "04" "IF without ELSE" \
    "$TESTS_DIR/04_if_only.pseudo" \
    0 'if (' '' ''

run_test "05" "IF … ELSE … ENDIF" \
    "$TESTS_DIR/05_if_else.pseudo" \
    0 'else {' '' ''

run_test "06" "WHILE countdown loop" \
    "$TESTS_DIR/06_while_loop.pseudo" \
    0 'while (' '' ''

run_test "07" "Accumulator (sum 1–10)" \
    "$TESTS_DIR/07_accumulator.pseudo" \
    0 'int sum;' '' ''

run_test "08" "Nested IF inside WHILE" \
    "$TESTS_DIR/08_nested_if_in_while.pseudo" \
    0 'while (' '' ''

run_test "09" "All 6 comparison operators" \
    "$TESTS_DIR/09_all_comparisons.pseudo" \
    0 'if ((a == b))' '' ''

run_test "10" "Complex exprs with parentheses: (3+4)*2" \
    "$TESTS_DIR/10_complex_expr.pseudo" \
    0 'int x;' '' ''

run_test "11" "Nested WHILE loops" \
    "$TESTS_DIR/11_nested_while.pseudo" \
    0 'int j;' '' ''

run_test "12" "SHOW with arithmetic expression" \
    "$TESTS_DIR/12_show_expr.pseudo" \
    0 'printf("%d' '' ''

run_test "13" "Self-assignment: SET x TO x" \
    "$TESTS_DIR/13_self_assign.pseudo" \
    0 'int x;' '' ''

run_test "14" "Division operator" \
    "$TESTS_DIR/14_division.pseudo" \
    0 '/ b)' '' ''

run_test "15" "Operator precedence (* before +, parens override)" \
    "$TESTS_DIR/15_operator_precedence.pseudo" \
    0 'int r;' '' ''

# ─── CATEGORY 2: DIAGNOSTIC CASES ────────────────────────────
sep
printf "${BOLD}  Category 2: Expected Warnings & Errors${RESET}\n"
sep

run_test "ERR-01" "Duplicate VAR — only ONE int x in output" \
    "$TESTS_DIR/ERR_01_duplicate_var.pseudo" \
    0 'int x;' '' "Error: Variable 'x' already declared."

run_test "ERR-02" "Undeclared variable in SET/SHOW" \
    "$TESTS_DIR/ERR_02_undeclared_set.pseudo" \
    0 'y = 99' '' "Warning: Assignment to undeclared variable 'y'."

run_test "ERR-03" "Syntax error: no START keyword" \
    "$TESTS_DIR/ERR_03_syntax_no_start.pseudo" \
    1 '' '' 'Translation Error:'

run_test "ERR-04" "Syntax error: WHILE without DONE" \
    "$TESTS_DIR/ERR_04_unclosed_while.pseudo" \
    1 '' '' 'Translation Error:'

run_test "ERR-05" "Syntax error: IF without ENDIF" \
    "$TESTS_DIR/ERR_05_unclosed_if.pseudo" \
    1 '' '' 'Translation Error:'

run_test "ERR-06" "Edge case: empty program body (START STOP)" \
    "$TESTS_DIR/ERR_06_empty_program.pseudo" \
    0 'int main()' '' ''

# ─── CATEGORY 3: INLINE EDGE CASES ───────────────────────────
sep
printf "${BOLD}  Category 3: Inline Edge Case Tests${RESET}\n"
sep

# Test: ERR-01 must emit exactly ONE "int x;" (no duplicate)
tmp=$(mktemp)
printf 'START\n  VAR x AS INT\n  VAR x AS INT\n  SHOW x\nSTOP\n' | "$COMPILER" 2>/dev/null > "$tmp"
count=$(grep -c 'int x;' "$tmp")
if [[ "$count" -eq 1 ]]; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-01] Duplicate decl: exactly 1 'int x;' in output\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-01] Duplicate decl: expected 1 'int x;', got $count")
    printf "  ${RED}✖  FAIL${RESET}  [EC-01] Duplicate decl: expected 1 'int x;', got %s\n" "$count"
fi
rm -f "$tmp"

# Test: operator precedence values
# 1 + 2 * 3 = 7  →  SET r TO 1 + 2 * 3  →  r = (1 + (2 * 3));
out=$(printf 'START\n  VAR r AS INT\n  SET r TO 1 + 2 * 3\n  SHOW r\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF '(2 * 3)'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-02] Precedence: 1 + 2*3 groups as 1 + (2*3)\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-02] Precedence: 1 + 2*3 should group as (2*3)")
    printf "  ${RED}✖  FAIL${RESET}  [EC-02] Precedence: wrong grouping\n"
    echo "$out" | sed 's/^/       /'
fi

# Test: empty program generates only main wrapper (no stmts, no crash)
out=$(printf 'START\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF 'int main()' && echo "$out" | grep -qF 'return 0;'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-03] Empty body: valid main() skeleton emitted\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-03] Empty body: expected main() skeleton")
    printf "  ${RED}✖  FAIL${RESET}  [EC-03] Empty body: bad output\n"
fi

# Test: #include <stdio.h> is always emitted
out=$(printf 'START\n  SHOW "hi"\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF '#include <stdio.h>'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-04] Output always starts with #include <stdio.h>\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-04] #include <stdio.h> missing from output")
    printf "  ${RED}✖  FAIL${RESET}  [EC-04] #include <stdio.h> missing\n"
fi

# Test: return 0; always present
if echo "$out" | grep -qF 'return 0;'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-05] Output always ends with return 0;\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-05] return 0; missing from output")
    printf "  ${RED}✖  FAIL${RESET}  [EC-05] return 0; missing\n"
fi

# Test: nested while produces two while loops
out=$(cat "$TESTS_DIR/11_nested_while.pseudo" | "$COMPILER" 2>/dev/null)
count=$(grep -c 'while (' <<< "$out")
if [[ "$count" -ge 2 ]]; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-06] Nested while: 2 'while (' blocks in output\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-06] Nested while: expected ≥2 while blocks, got $count")
    printf "  ${RED}✖  FAIL${RESET}  [EC-06] Nested while: expected 2 blocks, got %s\n" "$count"
fi

# Test: IF/ELSE both branches present
out=$(cat "$TESTS_DIR/05_if_else.pseudo" | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF 'if (' && echo "$out" | grep -qF 'else {'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-07] IF/ELSE: both 'if' and 'else' blocks generated\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-07] IF/ELSE: missing if or else block")
    printf "  ${RED}✖  FAIL${RESET}  [EC-07] IF/ELSE: missing block\n"
fi

# Test: GET emits scanf
out=$(printf 'START\n  VAR n AS INT\n  GET n\n  SHOW n\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF 'scanf('; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-08] GET generates scanf()\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-08] GET: scanf() missing from output")
    printf "  ${RED}✖  FAIL${RESET}  [EC-08] GET: scanf() missing\n"
fi

# Test: SHOW string emits printf %s
out=$(printf 'START\n  SHOW "hello"\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF '"%s\n"'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-09] SHOW string uses printf(\"%%s\\n\", ...)\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-09] SHOW string: wrong printf format")
    printf "  ${RED}✖  FAIL${RESET}  [EC-09] SHOW string: wrong printf format\n"
    echo "$out" | grep printf | sed 's/^/       /'
fi

# Test: SHOW variable emits printf %d
out=$(printf 'START\n  VAR n AS INT\n  SET n TO 1\n  SHOW n\nSTOP\n' | "$COMPILER" 2>/dev/null)
if echo "$out" | grep -qF '"%d\n"'; then
    PASS=$((PASS+1))
    printf "  ${GREEN}✔  PASS${RESET}  [EC-10] SHOW variable uses printf(\"%%d\\n\", ...)\n"
else
    FAIL=$((FAIL+1))
    FAILED_TESTS+=("[EC-10] SHOW var: wrong printf format")
    printf "  ${RED}✖  FAIL${RESET}  [EC-10] SHOW var: wrong printf format\n"
    echo "$out" | grep printf | sed 's/^/       /'
fi

# ─── SUMMARY ─────────────────────────────────────────────────
TOTAL=$((PASS + FAIL + WARN))
sep
printf "\n${BOLD}  Results: ${GREEN}%d passed${RESET}  ${RED}%d failed${RESET}  ${YELLOW}%d warned${RESET}  (total %d)\n\n" \
    "$PASS" "$FAIL" "$WARN" "$TOTAL"

if [[ ${#FAILED_TESTS[@]} -gt 0 ]]; then
    printf "${BOLD}${RED}  Failed tests:${RESET}\n"
    for t in "${FAILED_TESTS[@]}"; do
        printf "    ${RED}✖ %s${RESET}\n" "$t"
    done
    printf "\n"
fi

if [[ $FAIL -eq 0 ]]; then
    printf "  ${GREEN}${BOLD}All tests passed! ✔${RESET}\n\n"
else
    printf "  ${RED}${BOLD}%d test(s) need attention.${RESET}\n\n" "$FAIL"
    exit 1
fi
