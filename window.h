#pragma once

#include <gdk/x11/gdkx.h>
#include <gtk/gtk.h>

#include "rose.h"

typedef struct {
	double zoom;
	WebKitWebView *webview;
	WebKitWebInspector *inspector;
	WebKitFindOptions *findopts;
	GtkEventController *controller;
} RoseWebview;

typedef struct {
	unsigned    xid;
	short       tab;        /* current tab */
	GtkWidget   *tabs;      /* GtkNotebook with all the tabs */
	GtkWidget   *window;    /* app window */
	RoseWebview **webviews; /* array of tabs */
} RoseWindow;

RoseWindow *rose_window_new(GtkApplication *application, const char *options[]);

int rose_window_show(RoseWindow *window, const char *url);
void rose_window_close(RoseWindow *window);

RoseWebview *rose_webview_new();
