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
while getopts "oc:s:p:d:i:u:x:X:" opt; do
  case $opt in
    o)
      TEST_OCTAL=1
      ;;
    c|s|p|d|i|u|x|X)
      SPECIFIER_TO_TEST="$opt"
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))


# Process long options (remaining arguments)
for arg in "$@"; do
    case $arg in
        --bonus|-b)
            TEST_BONUS=1
            ;;
        --invalid)
            TEST_BONUS=1        # Extreme implies Bonus
            TEST_INVALID=1
            ;;
        --verbose|-v)
            TEST_VERBOSE=1      # Set verbose output
            ;;
        --pass|--pok)
            TEST_PASS=1         # Set print OK results
            ;;
        --valgrind|--mem|--memcheck|--strict|-m)
            RUN_VALGRIND=1
            ;;
        *)
            echo "Warning: Unknown argument '$arg' ignored." >&2
            ;;
    esac
done

rm -rf *.o tester

# Rebuild libftprintf in the parent directory
make -C .. -s fclean
make -C .. -s


# --- 4. Define Compiler Parameters (Updated) ---
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

# Check if compilation was successful before running
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