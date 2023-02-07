#!/bin/sh

CC=clang
SRC=rose.c
REQS= #./plugins/*/*.c
DEPS='webkit2gtk-4.0'

INCS=`pkg-config --cflags ${DEPS}`
LIBS=`pkg-config --libs ${DEPS}`

# Optional adblocking depends on https://github.com/jun7/wyebadblock
WYEBAB='-L/usr/lib/wyebrowser/adblock.so'

cp -f config.def.h config.h
$CC $INCS $REQS $SRC -o rose $LIBS $WYEBAB
