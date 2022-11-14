CC=clang
SRC=rose.c
DEPS=('webkit2gtk-4.0' 'libadwaita-1')

INCS=`pkg-config --cflags ${DEPS[@]}`
LIBS=`pkg-config --libs ${DEPS[@]}`

$CC $INCS $LIBS $SRC -o rose
