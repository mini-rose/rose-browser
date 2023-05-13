#include "search.h"
#include "lua.h"
#include "window.h"
#include "webview.h"

#include <lua.h>
#include <gtk/gtk.h>

static GtkWidget *last_focused = NULL;

void rose_search_cb(GtkWidget *entry, gpointer data)
{
	RoseWindow *rw = rose_window_get();
	GtkWidget *dialog = GTK_WIDGET(data);
	const char *uri = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	gtk_widget_destroy(dialog);
	gtk_window_set_focus(rw->window, last_focused);
	rose_webview_load_uri(uri);
	free((char *)uri);
}

int rose_search_open(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	last_focused = gtk_window_get_focus(rw->window);
	GtkWidget *dialog = gtk_dialog_new();

	gtk_window_set_transient_for(GTK_WINDOW(dialog), rw->window);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

	GtkWidget *entry = gtk_entry_new();
	gtk_container_add(
		GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
	    entry
	);

	g_signal_connect(entry, "activate", G_CALLBACK(rose_search_cb), dialog);

	gtk_widget_show_all(dialog);

	return 0;
}

void rose_search_lua_api(lua_State *L)
{
	rose_lua_value("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_search_open);
	lua_setfield(L, -2, "search");
}
