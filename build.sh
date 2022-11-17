CC=clang
SRC=rose.c
DEPS=('webkit2gtk-4.0')

INCS=`pkg-config --cflags ${DEPS[@]}`
LIBS=`pkg-config --libs ${DEPS[@]}`

# Optional adblocking depends on https://github.com/jun7/wyebadblock
WYEBAB='-L/usr/lib/wyebrowser/adblock.so'

$CC $INCS $LIBS $SRC $WYEBAB -o rose
