-include build/makedeps.mk
include config.mk

MAKEFLAGS += -j$(nproc) -r -R
SOURCE := $(shell find src -type f -name '*.c')
OBJECT := $(patsubst src/%.c,build/%.o,$(SOURCE))
SRCDIR := $(shell find src -type d -wholename 'src/*' | sed 's/^src/build/g')
OUTPUT := build/rose

BUILDTYPE ?= DEBUG

ifeq ($(BUILDTYPE), DEBUG)
	CFLAGS += -O0 -ggdb -DDEBUG=1
else ifeq ($(BUILDTYPE), RELEASE)
	CFLAGS += -Ofast
endif

rose: build build/makedeps.mk $(OUTPUT)

build:
	@mkdir -p build $(SRCDIR)

build/makedeps.mk: build $(SOURCE)
	$(CC) -MM $(CFLAGS) $(SOURCE) | \
		sed 's/\b\([a-zA-Z0-9_]*\.o\)\b/build\/\1/g' > build/makedeps.mk

run: rose
	$(OUTPUT)

install: rose
	cp -f $(OUTPUT) $(BINDIR)/rose

uninstall:
	rm -f $(BINDIR)/mcc

clean:
	rm -rf build

compile_flags.txt: makefile
	echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt

$(OUTPUT): $(OBJECT)
	@printf " \033[32m LD \033[m $(shell basename $@)\n"
	@$(CC) -o $@ $(USE_LD) $(CFLAGS) $(LDFLAGS) $^

build/%.o: src/%.c
	@printf " \033[33m CC \033[m $(shell basename $@)\n"
	@$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: install uninstall clean
.SILENT: build/makedeps.mk
.DEFAULT_GOAL := rose
