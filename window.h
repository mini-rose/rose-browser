#pragma once

#include <gdk/x11/gdkx.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "rose.h"

typedef struct {
	double zoom;
	WebKitWebView *webview;
	WebKitWebInspector *inspector;
	WebKitFindOptions *findopts;
	GtkEventController *controller;
	GtkEventController *gestures;
} RoseWebview;

typedef struct {
	unsigned    xid;
	short       tab;        /* current tab */
	GtkWidget   *tabview;   /* GtkNotebook with all the tabs */
	GtkWidget   *window;    /* app window */
	RoseWebview **tabs;     /* array of tabs */
} RoseWindow;

RoseWindow *rose_window_new(GtkApplication *a, char *uri);

int rose_window_show(RoseWindow *w);
void rose_window_close(RoseWindow *window);

RoseWebview *rose_webview_new();
