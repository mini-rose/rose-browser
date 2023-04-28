-include build/makedeps.mk
include config.mk

MAKEFLAGS += -j$(nproc)
SOURCE := $(shell find src -type f -name '*.c')
OBJECT := $(patsubst src/%.c,build/%.o,$(SOURCE))
SRCDIR := $(shell find src -type d -wholename 'src/*' | sed 's/^src/build/g')
OUTPUT := build/rose

ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -ggdb -DDEBUG=1 -fsanitize=address
else
	CFLAGS += -Ofast
endif

rose: build build/makedeps.mk $(OUTPUT)

build:
	mkdir -p build $(SRCDIR)

build/makedeps.mk: build $(SOURCE)
	$(CC) -MM $(CFLAGS) $(SOURCE) | \
		sed 's/\b\([a-zA-Z0-9_]*\.o\)\b/build\/\1/g' > build/makedeps.mk

run:
	make
	$(OUTPUT)

install:
	cp -f $(OUTPUT) $(BINDIR)/mcc

uninstall:
	$(RM) $(BINDIR)/mcc

clean:
	$(RM) -r build

compile_flags.txt: makefile
	echo $(CFLAGS) | tr ' ' '\n' > compile_flags.txt

$(OUTPUT): $(OBJECT)
	$(CC) -o $@ $(USE_LD) $(CFLAGS) $(LDFLAGS) $^

build/%.o: src/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: install uninstall clean
.SILENT: build/makedeps.mk
.DEFAULT_GOAL := rose
