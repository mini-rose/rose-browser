#pragma once
#include "keyconf.h"

static const int dark_mode = 1;
static const char *cachedir = "/home/fenze/.cache/rose/";
static const char *homepage = "https://duckduckgo.com";

#define MODKEY GDK_CONTROL_MASK

static Key keys[] = {
	{ MODKEY,         GDK_KEY_h,     goback     },
	{ MODKEY,         GDK_KEY_l,     goforward  },
	{ MODKEY,         GDK_KEY_y,     copy_url   },
	{ MODKEY,         GDK_KEY_p,     paste_url  },
	{ MODKEY,         GDK_KEY_e,     search     },
	{ MODKEY,         GDK_KEY_equal, zoomin     },
	{ MODKEY,         GDK_KEY_minus, zoomout    },
	{ MODKEY,         GDK_KEY_i,     inspector  },
	{ MODKEY,         GDK_KEY_j,     down       },
	{ MODKEY,         GDK_KEY_k,     up         },
	{ 0,              GDK_KEY_F11,   fullscreen }
};
