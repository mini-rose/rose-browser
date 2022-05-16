#pragma once

#include "window.h"
#include "webview.h"
#include "config.h"

#include <gtk/gtk.h>
#include <gdk/x11/gdkx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <webkit2/webkit2.h>
#include <sys/wait.h>

enum {
	AtomFind,
	AtomGo,
	AtomUri,
	AtomUTF8,
	AtomLast
};

extern Display *glob_dpy;  /* declared in rose.c */

const char* getatom(int a);
void setatom(int a, const char *v);
