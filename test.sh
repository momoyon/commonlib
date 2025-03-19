#!/bin/sh

export SRC_SUFFIX=.c
export TESTS_DIR=./tests
export BUILD_CMD="gcc -o {test_name} {test_name}.{src_suffix}"
export RUN_CMD="./{test_name}"

./test.py $*
