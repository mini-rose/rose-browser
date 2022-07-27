# -*- indent-tabs-mode: t -*-
CC       ?= cc
CFLAGS   := `pkgconf --cflags webkit2gtk-5.0`
LIBS     := `pkgconf --libs webkit2gtk-5.0`
PREFIX   ?= /usr/local

all: config.h rose

rose:
	$(CC) -fPIC -Os -o rose *.c $(CFLAGS) $(LIBS)

debug: config.h
	$(CC) -g -o rose *.c $(CFLAGS) $(LIBS) -Wall -Wextra \
		-fsanitize=address -Wpedantic

config.h:
	[ -f "$@" ] || cp config.def.h $@

install: all
	cp -f ./rose $(PREFIX)/bin/rose
	cp -f ./scripts/dmenu_rose.sh $(PREFIX)/bin/dmenu_rose

uninstall:
	rm -f $(PREFIX)/bin/rose $(PREFIX)/bin/dmenu_rose

clean:
	rm -f rose

clean-all: clean
	rm -f config.h

flags:
	echo $(CFLAGS) | sed 's/ /\n/g' > compile_flags.txt

.PHONY: all rose clean clean-all install uninstall flags config.h
.SILENT: all rose clean clean-all install uninstall flags config.h
