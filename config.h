/*
 * Copyright (c) 2022 mini-rose
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and any associated documentation
 * files to modify, copy, merge, publish, distribute and/or
 * sublicense copies of this sotware for their own use.
 * This code does not come with any warranty.
 *
 * Author: fenze <contact@fenze.dev>
 */

#include <gdk/gdkkeysyms.h>

/* See more: https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html */
#define WEBKIT \
	"enable-back-forward-navigation-gestures", true, \
	"enable-developer-extras", true

#define GTK \
	"gtk-application-prefer-dark-theme", true

#define HOME "https://duckduckgo.com"
#define SEARCH "https://duckduckgo.com/?q=%s"
#define CACHE_DIR "/home/fenze/.cache/rose"

#define KEY(x) GDK_KEY_ ## x
#define ZOOM 1 /* Starting zoom level */
#define ZOOM_VAL .1 /* Zooming value in zoomin/zoomout functions */
#define BG_COLOR "#1E1E2E"

typedef enum {
	goback, goforward,
	refresh, refresh_force,
	back_to_home, toggle_fullscreen,
	zoomin, zoomout, zoom_reset,
	next_tab, prev_tab, close_tab,
	show_searchbar, show_finder,
	finder_next, finder_prev
} func;

#define SHIFT 1 << 0
#define CTRL  1 << 2
#define ALT   1 << 3

static struct Key {
	unsigned mod;
	unsigned key;
	func id;
} keys[] = {
	{ CTRL, KEY(h), goback    },
	{ CTRL, KEY(l), goforward },

	/* Refresh keybinds */
	{ CTRL,         KEY(r), refresh       },
	{ CTRL | SHIFT, KEY(R), refresh_force },

	{ CTRL | SHIFT, KEY(H), back_to_home },

	{ CTRL, KEY(equal), zoomin },
	{ CTRL, KEY(minus), zoomout },
	{ CTRL, KEY(0), zoom_reset },

	{ ALT, KEY(h), prev_tab },
	{ ALT, KEY(l), next_tab },

	{ CTRL, KEY(w), close_tab },

	{ 0, KEY(F11), toggle_fullscreen },

	{ CTRL, KEY(e), show_searchbar },
	{ CTRL, KEY(f), show_finder },

	{ CTRL, KEY(n), finder_next },
	{ CTRL | SHIFT, KEY(N), finder_prev }
};
