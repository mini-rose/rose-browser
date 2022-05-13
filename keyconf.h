#pragma once

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdbool.h>

#define MODKEY GDK_CONTROL_MASK
#define MODSFT   5
#define NOMODK   0

#define CACHE      0
#define HOMEPAGE   1

#define DEFAULT    NULL

#define HEIGHT     0
#define WIDTH      1
#define DARKMODE   2
#define SCROLLBARS 3

#define KEYBINDS   static Key keys[] =
#define APPERANCE  static int appearance[] =
#define OPTIONS    static char *options[] =

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
	reloadforce,
	LAST_FUNC
};
