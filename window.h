#pragma once

#include "rose.h"

#include <gtk/gtk.h>
#include <gdk/x11/gdkx.h>

#define ROSE_TYPE_WINDOW rose_window_get_type()

G_DECLARE_FINAL_TYPE(RoseWindow, rose_window, ROSE, WINDOW, GtkApplicationWindow)

RoseWindow* rose_window_new();
guint rose_window_show(GtkApplication *app, RoseWindow *window, const char *url);
gboolean rose_window_close(RoseWindow *window);

void rose_window_set_webview(RoseWindow *window, GtkWidget *webview);

GActionGroup* rose_window_get_action_group(RoseWindow *window,
                                           const char *prefix);
