#pragma once

#include <gdk/x11/gdkx.h>
#include <gtk/gtk.h>

#include "rose.h"

typedef struct {
    double zoom;
    WebKitWebView *webview;
    WebKitWebInspector *inspector;
    WebKitFindOptions *findopts;
} RoseWebview;

typedef struct {
    unsigned xid;
    short tab;
    GtkWidget *window;
    RoseWebview **webviews;
} RoseWindow;

RoseWindow *rose_window_new(GtkApplication *application);

int rose_window_show(RoseWindow *window, const char *url);
void rose_window_close(RoseWindow *window);
