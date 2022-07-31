CC     ?= cc
PREFIX := /usr/local

all: config.h rose

rose: rose.o
	$(CC) `pkgconf --libs webkit2gtk-5.0` -o $@ $?

rose.o: rose.c
	$(CC) `pkgconf --cflags webkit2gtk-5.0` -c $<

config.h:
	cp config.def.h $@

install: rose
	cp -f ./rose $(PREFIX)/bin/rose

uninstall:
	rm -f $(PREFIX)/bin/rose

clean:
	rm -f rose rose.o

.SILENT: config.h rose rose.o
.PHONY: all install uninstall clean
