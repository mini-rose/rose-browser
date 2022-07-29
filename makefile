CC     ?= cc
PREFIX := /usr/local

rose: config.h rose.o
	$(CC) `pkgconf --libs webkit2gtk-5.0` -o $@ $@.o

rose.o: rose.c
	$(CC) `pkgconf --cflags webkit2gtk-5.0` -c $<

config.h:
	cp config.def.h $@

.PHONY:

install: rose
	cp -f ./rose $(PREFIX)/bin/rose

uninstall:
	rm -f $(PREFIX)/bin/rose

clean:
	rm -f rose rose.o

.SILENT: rose rose.o
