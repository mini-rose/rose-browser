/*
 * This file is part of mini-rose/rose library.
 * Project is under the terms of the MIT License.
 *
 * Copyright (c) 2023 mini-rose
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "rose.h"

/* See more: https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html */
#define WEBKIT \
	"enable-back-forward-navigation-gestures", true, \
	"enable-developer-extras", true, \
	"enable-smooth-scrolling", false

#define GTK \
	"gtk-application-prefer-dark-theme", true, \
	"gtk-enable-animations", false

#define HOME "https://duckduckgo.com"
#define SEARCH "https://duckduckgo.com/?q=%s"
#define CACHE_DIR "/home/fenze/.cache/rose"

#define ZOOM 1 /* Starting zoom level */
#define ZOOM_VAL .1 /* Zooming value in zoomin/zoomout functions */
#define BG_COLOR "#1E1E2E"

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
