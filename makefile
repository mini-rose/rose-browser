CC = clang
CFLAGS = `pkg-config --cflags gtk4 webkit2gtk-5.0`
LIBS   = `pkg-config --libs gtk4 webkit2gtk-5.0`

all:
	${CC} -fPIC -g -O3 -o rose *.c $(CFLAGS) $(LIBS)
	strip ./rose

install: all
	cp -uf ./rose /usr/local/bin/rose

clean:
	rm -f rose compile_flags.txt

flags:
	echo $(CFLAGS) | sed 's/ /\n/g' > compile_flags.txt

.SILENT: all clean flags
