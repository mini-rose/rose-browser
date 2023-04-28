VERSION = 3.0

# Programs
CC ?= cc
PKGCONFIG := pkg-config

# Paths
PREFIX := /usr/local
LIBPREFIX := $(PREFIX)/lib
LIBDIR := $(LIBPREFIX)/rose


# Includes and libraries
WEBKIT_INCS := `$(PKGCONFIG) --cflags webkitgtk-6.0 webkitgtk-web-process-extension-6.0`
WEBKIT_LIBS := `$(PKGCONFIG) --libs webkitgtk-6.0 webkitgtk-web-process-extension-6.0`

CFLAGS  := -Wall -Wextra -Iinclude \
		   -march=native -flto -pipe \
		   -DVERSION=\"$(VERSION)\" \
		   $(WEBKIT_INCS)

LDFLAGS := $(WEBKIT_LIBS) -llua
