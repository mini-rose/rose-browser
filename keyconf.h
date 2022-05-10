#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#define NONE 4

typedef struct {
	unsigned modkey;
	unsigned keycod;
	unsigned funcid;
} Key;

enum {
	goback,
	goforward,
	copy_url,
	paste_url,
	fullscreen
};
