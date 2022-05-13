#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#define MODSHIFT 5

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
	find,
	findnext,
	findprev,
	zoomin,
	zoomout,
	zoomreset,
	down,
	up,
	reload,
	reloadforce
};
