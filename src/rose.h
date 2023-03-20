#pragma once

#include <webkit2/webkit2.h>
#include <gdk/gdkkeysyms.h>

typedef enum {
	goback, goforward,
	refresh, refresh_force,
	back_to_home, toggle_fullscreen,
	zoomin, zoomout, zoom_reset,
	next_tab, prev_tab, close_tab,
	show_searchbar, show_finder,
	finder_next, finder_prev
} func;

#define CACHE                                                \
	"base-cache-directory", CACHE_DIR,                   \
	"base-data-directory", CACHE_DIR,                    \
	"disk-cache-directory", CACHE_DIR,                   \
	"dom-cache-directory", CACHE_DIR,                    \
	"hsts-cache-directory", CACHE_DIR,                   \
	"indexeddb-directory", CACHE_DIR,                    \
	"itp-directory", CACHE_DIR,                          \
	"local-storage-directory", CACHE_DIR,                \
	"offline-application-cache-directory", CACHE_DIR,    \
	"service-worker-registrations-directory", CACHE_DIR, \
	NULL

enum { _SEARCH, _FIND };

#define SHIFT 1 << 0
#define CTRL  1 << 2
#define ALT   1 << 3
#define KEY(x) GDK_KEY_ ## x

struct settings {
	double zoom;
	double zoom_val;
	char *background;
};
