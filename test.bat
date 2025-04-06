@echo off

set SRC_SUFFIX=.c
set TESTS_DIR=./tests
set BUILD_CMD=gcc -o {test_name} {test_name}.{src_suffix}
set RUN_CMD=".\{test_name}"

python .\test.py %*
