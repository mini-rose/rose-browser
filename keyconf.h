#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#define SHIFT GDK_SHIFT_MASK

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
	fullscreen,
	inspector,
	search,
	zoomin,
	zoomout
};
