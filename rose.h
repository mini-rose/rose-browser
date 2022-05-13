#pragma once

#include "window.h"
#include "webview.h"
#include "config.h"

#include <gtk/gtk.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>
#include <X11/Xatom.h>
#include <gdk/x11/gdkx.h>
#include <webkit2/webkit2.h>
#include <stdlib.h>

enum { AtomFind, AtomGo, AtomUri, AtomUTF8, AtomLast };
static Atom atoms[AtomLast];
static Display *dpy;
const char* getatom(int a);
void setatom(int a, const char *v);
static int spair[2];
