set -e # Exit on any error

# Run static analysis
cppcheck test src lib/utils lib/Drivers lib/Devices lib/rwmutex

# Compile and run functional test for non-optimized native environment
platformio run -e native_ci
python -m unittest -v test_dummy_fsw

# Verify compilation and unit tests for optimized native environment
platformio run -e native
platformio test -e native

# Compile and run HOOTL/Flight Teensy environments
platformio run -e teensy35_cli_hootl
platformio run -e teensy36_cli_hootl
platformio run -e teensy_cli_preflight

# Compile hardware functional tests
platformio run -e teensy_35_test_quake
platformio run -e teensy_35_test_quake_no_network
platformio run -e teensy_35_test_quake_network
platformio run -e teensy_36_test_quake
platformio run -e teensy_36_test_quake_no_network
platformio run -e teensy_36_test_quake_network

# Run tests for non-optimized native environment. These are
# identical to the tests for the optimized environment,
# so they're lower-priority in this CI build step (they're less
# likely to fail the build sequence.)
#
# We also need this step to run last so that code coverage is available.
platformio test -e native_ci

# Check for memory mismanagement
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose .pio/build/native_ci/program