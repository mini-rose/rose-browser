# -*- indent-tabs-mode: t -*-
CC       ?= clang
CFLAGS   = `pkg-config --cflags gtk4 webkit2gtk-5.0 x11`
LIBS     = `pkg-config --libs gtk4 webkit2gtk-5.0 x11`
OPTIONS  = -Dgtk_doc=false -Dintrospection=false \
					 -DG_DISABLE_CAST_CHECKS \
					 -Dcloudproviders=false \
					 -DGTK_DISABLE_DEPRECATED \
					 -DGDK_PIXBUF_DISABLE_DEPRECATED \
					 -DG_DISABLE_DEPRECATED -DENABLE_GLES2=ON \
					 -DENABLE_GEOLOCATION=OFF -DENABLE_PLUGIN_PROCESS_GTK2=OFF \
					 -DPORT=GTK \
					 -Dforce_posix_threads=true \
					 -Dman=false \
					 -Dxinerama=false \
					 -Dcolord=false \
					 -Db_coverage=false \
					 -Ddebug=false

PREFIX   ?= /usr/local

all: config.h rose

rose:
	$(CC) -fPIC -O3 -o rose *.c $(CFLAGS) $(LIBS) $(OPTIONS)
	strip ./rose

debug:
	$(CC) -fPIC -o rose *.c $(CFLAGS) $(LIBS) -Wall -Wextra

config.h:
	[ -f "$@" ] || cp config.def.h $@

install: all
	cp -f ./rose $(PREFIX)/bin/rose
	cp -f ./scripts/dmenu_rose.sh $(PREFIX)/bin/dmenu_rose

uninstall:
	rm -f $(PREFIX)/bin/rose $(PREFIX)/bin/dmenu_rose

clean:
	rm -f rose compile_flags.txt

clean-all: clean
	rm -f config.h

flags:
	echo $(CFLAGS) | sed 's/ /\n/g' > compile_flags.txt

.PHONY: all clean clean-all install uninstall flags config.h
.SILENT: all clean clean-all install uninstall flags config.h
