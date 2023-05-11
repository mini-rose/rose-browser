#include "window.h"
#include "debug.h"
#include "webview.h"
#include "lua.h"
#include <stdbool.h>


/* TODO: allow to insert multiple splits */

void rose_window_vsplit(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkWidget *splitter = gtk_window_get_focus(rw->window);

	while (splitter && !GTK_IS_PANED(splitter)) {
		splitter = gtk_widget_get_parent(splitter);
	}

	if (splitter != NULL) {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(splitter), GTK_ORIENTATION_VERTICAL);
		gtk_paned_pack2(GTK_PANED(splitter), GTK_WIDGET(rose_webview_new()), true, false);
	}

	gtk_widget_show_all(GTK_WIDGET(rw->window));
}

void rose_window_hsplit(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkWidget *splitter = gtk_window_get_focus(rw->window);
	while (splitter && !GTK_IS_PANED(splitter)) {
		splitter = gtk_widget_get_parent(splitter);
	}

	if (splitter != NULL) {
		gtk_orientable_set_orientation(GTK_ORIENTABLE(splitter), GTK_ORIENTATION_HORIZONTAL);
		gtk_paned_pack2(GTK_PANED(splitter), GTK_WIDGET(rose_webview_new()), true, false);
	}

	gtk_widget_show_all(GTK_WIDGET(rw->window));
}
