# Copyright (c) 2023 fenze

#  Targets:
#
#   make		build the browser
#   make clean		remove the build dir
#   make install	install the rose binary
#   make uninstall	remove the rose binary


# -- Options --

CC ?= cc

PREFIX   := /usr
BINDIR   := $(PREFIX)/bin
BUILDDIR := build
PKGCONFIG := pkg-config

CFLAGS  := -std=c17 -Werror -Wextra -O3 \
	   -march=native -fomit-frame-pointer -funroll-loops

CFLAGS += `$(PKGCONFIG) --cflags webkit2gtk-4.0`
LDFLAGS := `$(PKGCONFIG) --libs webkit2gtk-4.0`

SOURCE := src/rose.c
OUTPUT := $(BUILDDIR)/rose


# -- Targets --

$(OUTPUT): $(BUILDDIR) $(SOURCE)
	$(CC) $(LDFLAGS) $(CFLAGS) $(SOURCE) -o $@

$(BUILDDIR):
	$(RM) $(BUILDDIR)
	mkdir -p $(BUILDDIR)

install: $(OUTPUT)
	cp -f $(OUTPUT) $(BINDIR)

uninstall:
	$(RM) $(BINDIR)/$(notdir $(OUTPUT))

clean:
	$(RM) -r $(BUILDDIR)
	$(RM) compile_flags.txt

compile_flags.txt:
	echo $(CFLAGS) | tr " " "\n" > compile_flags.txt

.PHONY: install uninstall clean compile_flags.txt
