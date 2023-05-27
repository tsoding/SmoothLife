#!/bin/sh

set -xe

clang -O3 -Wall -Wextra -o main main.c -lm
