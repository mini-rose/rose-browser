#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

typedef struct {
	unsigned modkey;
	unsigned keycod;
	unsigned funcid;
} Key;

enum {
	goback,
	goforward,
	copy_url,
	paste_url
};
