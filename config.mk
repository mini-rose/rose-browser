VERSION := 3.0

# Programs
CC ?= cc
LD := $(if $(shell which mold),mold,ld)
PKGCONFIG := pkg-config

# Paths
PREFIX := /usr/local
LIBPREFIX := $(PREFIX)/lib
LIBDIR := $(LIBPREFIX)/rose
BINDIR := $(PREFIX)/bin

# Gtk version (3|4)
# Default is 3
GTK ?= 4

# Includes and libraries
ifeq ($(GTK), 4)
	WEBKIT_INCS := `$(PKGCONFIG) --cflags webkitgtk-6.0 webkitgtk-web-process-extension-6.0`
	WEBKIT_LIBS := `$(PKGCONFIG) --libs webkitgtk-6.0 webkitgtk-web-process-extension-6.0`
else ifeq ($(GTK), 3)
	WEBKIT_INCS := `$(PKGCONFIG) --cflags webkit2gtk-4.0 webkit2gtk-web-extension-4.0`
	WEBKIT_LIBS := `$(PKGCONFIG) --libs webkit2gtk-4.0 webkit2gtk-web-extension-4.0`
endif

LUA_INCS := `$(PKGCONFIG) --cflags lua`
LUA_LIBS := `$(PKGCONFIG) --libs lua`

CFLAGS  := -Wall -Wextra -Iinclude \
		   -march=native -pipe \
		   -DVERSION=\"$(VERSION)\" -DGTK=$(GTK) \
		   $(WEBKIT_INCS) $(LUA_INCS)

LDFLAGS := $(WEBKIT_LIBS) $(LUA_LIBS) -fuse-ld=$(LD)
