#!/bin/bash

# Default flags
SPECIFIER_TO_TEST=""
TEST_OCTAL=0
TEST_BONUS=0      # Default: Mandatory only
TEST_INVALID=0    # Default: Extreme cases off
TEST_VERBOSE=0    # Enables VERBOSE
TEST_PASS=0       # Enables PRINT_OK (for successful tests)
RUN_VALGRIND=0

# --- Argument Parsing ---
TEMP=$(getopt -o ocspdiuxXbvm -l bonus,invalid,verbose,pass,pok,valgrind,mem,memcheck,strict -- "$@")
if [ $? -ne 0 ]; then
  echo "Failed to parse options." >&2
  exit 1
fi
eval set -- "$TEMP"

while true; do
  case "$1" in
    -o) TEST_OCTAL=1; shift ;;
    -c|-s|-p|-d|-i|-u|-x|-X) SPECIFIER_TO_TEST="${1#-}"; shift ;;  # keep last char
    -b|--bonus) TEST_BONUS=1; shift ;;
    -v|--verbose) TEST_VERBOSE=1; shift ;;
    -m|--valgrind|--mem|--memcheck|--strict) RUN_VALGRIND=1; shift ;;
    --) shift; break ;;
    *) echo "Internal error: $1" >&2; exit 1 ;;
  esac
done

rm -rf *.o tester

# --- Rebuild libftprintf in the parent directory --- 
make -C .. -s fclean
make -C .. -s


# --- Define Compiler Parameters (Updated) ---
# Inject all flags (OCTAL, SPECIFIER, BONUS, INVALID, VERBOSE, PASS)
PARAMS="-DTEST_OCTAL=$TEST_OCTAL                \
        -DTEST_SPECIFIER=\"$SPECIFIER_TO_TEST\" \
        -DTEST_FLAGS=$TEST_BONUS                \
        -DTEST_INVALID=$TEST_INVALID            \
        -DVERBOSE=$TEST_VERBOSE                 \
        -DPRINT_OK=$TEST_PASS"


cc -w $PARAMS ft_printf_tester.c \
    -L ..                        \
    -lftprintf                   \
    -o tester

# --- Check if compilation was successful before running --- 
if [ $? -eq 0 ]; then
    echo "Tests compiled successfully. Running..."
    if [ $RUN_VALGRIND -eq 1 ]; then
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./tester
    else
        ./tester
    fi
else
    echo "Compilation failed."
fi