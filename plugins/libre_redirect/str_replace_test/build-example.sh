#!/bin/bash
CC=gcc

FLAGS="-std=c99 -Wall -lm"

SRC=example.c
REQS="../str_replace_start.c ../libre_redirect.c"

echo -e "\n\n\n"
$CC $FLAGS $SRC $REQS -o example

