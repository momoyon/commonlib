#!/bin/sh

export SRC_SUFFIX=.c
export TESTS_DIR=./tests
export BUILD_CMD="gcc -std=c11 -o {test_name} {test_name}.{src_suffix}"
export RUN_CMD="./tests/{test_name}"

python ./test.py $*
