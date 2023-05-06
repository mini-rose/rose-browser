VERSION := 3.0

# Programs
CC ?= cc
PKGCONFIG := pkg-config

# Paths
PREFIX := /usr/local
LIBPREFIX := $(PREFIX)/lib
LIBDIR := $(LIBPREFIX)/rose

# Gtk version (3|4)
# Default is 3
GTK ?= 3

# Includes and libraries
ifeq ($(GTK), 4)
	WEBKIT_INCS := `$(PKGCONFIG) --cflags webkitgtk-6.0 webkitgtk-web-process-extension-6.0`
	WEBKIT_LIBS := `$(PKGCONFIG) --libs webkitgtk-6.0 webkitgtk-web-process-extension-6.0`
else ifeq ($(GTK), 3)
	WEBKIT_INCS := `$(PKGCONFIG) --cflags webkit2gtk-4.0 webkit2gtk-web-extension-4.0`
	WEBKIT_LIBS := `$(PKGCONFIG) --libs webkit2gtk-4.0 webkit2gtk-web-extension-4.0`
endif

CFLAGS  := -Wall -Wextra -Iinclude \
		   -march=native -flto -pipe \
		   -DVERSION=\"$(VERSION)\" -DGTK=$(GTK) \
		   $(WEBKIT_INCS)

LDFLAGS := $(WEBKIT_LIBS) -llua
