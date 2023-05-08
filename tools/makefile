CC ?= cc
PKGCONFIG = pkg-config
GTK := 3

CFLAGS := -DGTK=$(GTK)

ifeq ($(GTK), 4)
	GTK_INCS = `$(PKGCONFIG) --cflags gtk4`
	GTK_LIBS = `$(PKGCONFIG) --libs gtk4`
else ifeq ($(GTK), 3)
	GTK_INCS = `$(PKGCONFIG) --cflags gtk+-3.0 gdk-3.0`
	GTK_LIBS = `$(PKGCONFIG) --libs gtk+-3.0 gdk-3.0`
endif

keyname: keyname.c
	$(CC) -o keyname keyname.c $(CFLAGS) $(GTK_INCS) $(GTK_LIBS)

compile_flags.txt:
	echo $(CFLAGS) $(GTK_INCS) | tr " " "\n" > compile_flags.txt

clean:
	$(RM) keyname

cleanall: clean
	$(RM) compile_flags.txt

.PHONY: compile_flags.txt clean
