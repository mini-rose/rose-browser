CC=clang
SRC=rose.c
DEPS=('webkit2gtk-4.0')

INCS=`pkg-config --cflags ${DEPS[@]}`
LIBS=`pkg-config --libs ${DEPS[@]}`

$CC $INCS $LIBS $SRC -o rose
