#pragma once

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <gdk/x11/gdkx.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <webkit2/webkit2.h>

#include "window.h"
#include "config.h"

enum { AtomFind, AtomGo, AtomUri, AtomUTF8, AtomLast };

extern Display *glob_dpy; /* declared in rose.c */

const char *getatom(int a);
void setatom(int a, const char *v);
