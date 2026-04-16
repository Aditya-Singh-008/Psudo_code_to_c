#!/usr/bin/env bash
# =============================================================
#  test.sh — Omni-Language Translation Verification Suite
#  Usage (from WSL):
#    cd /mnt/d/Projects/Psudo_code_to_c/tests
#    bash test.sh
# =============================================================

COMPILER="/mnt/d/Projects/Psudo_code_to_c/logic/my_compiler"
TESTS_DIR="/mnt/d/Projects/Psudo_code_to_c/tests"

# Colours
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

LANGS=("c" "python" "java")
PASS=0; FAIL=0

printf "\n${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}\n"
printf "${BOLD}  Omni-Language Translation Test Suite${RESET}\n"
printf "${BOLD}${CYAN}══════════════════════════════════════════════════════${RESET}\n\n"

# Only loop through the valid pseudocode test files (e.g. 01_hello_world.pseudo)
# We skip the ERR_ testing files here since this is specifically testing successful AST parsing for the new langs
for file in "$TESTS_DIR"/[0-1]*.pseudo; do
    filename=$(basename "$file")
    printf "${BOLD}File: ${YELLOW}%s${RESET}\n" "$filename"
    
    for lang in "${LANGS[@]}"; do
        # Compile checking stderr for crashes
        stdout=$("$COMPILER" "$lang" < "$file" 2>/tmp/_test_stderr)
        exit_code=$?
        stderr=$(cat /tmp/_test_stderr)
        
        # Check if 1) exit code failed or 2) grammar / parsing completely crashed
        if [[ "$exit_code" != "0" ]] || grep -qF "Syntax Error" <<< "$stdout" || grep -qF "Parsing failed" <<< "$stderr"; then
            printf "      ${RED}✖ FAIL${RESET}  → %-7s (Parser crashed)\n" "$lang"
            FAIL=$((FAIL + 1))
        else
            # Ensure output is actually being generated for the target lang
            if [[ -n "$stdout" ]]; then
                printf "      ${GREEN}✔ PASS${RESET}  → %-7s\n" "$lang"
                PASS=$((PASS + 1))
            else
                printf "      ${RED}✖ FAIL${RESET}  → %-7s (Generated blank file)\n" "$lang"
                FAIL=$((FAIL + 1))
            fi
        fi
    done
    printf "  ──────────────────────────────────────────\n"
done

TOTAL=$((PASS + FAIL))
printf "\n${BOLD}  Results: ${GREEN}%d valid translations${RESET}  ${RED}%d crashes${RESET}  (total %d)\n\n" "$PASS" "$FAIL" "$TOTAL"

if [[ $FAIL -eq 0 ]]; then
    printf "  ${GREEN}${BOLD}All syntax tree translations passed flawlessly! ✔${RESET}\n\n"
else
    exit 1
fi
