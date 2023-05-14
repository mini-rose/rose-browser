#include "search.h"
#include "lua.h"
#include "window.h"
#include "webview.h"

#include <lua.h>
#include <gtk/gtk.h>

static GtkWidget *last_focused = NULL;

#if GTK == 4
static GtkEntryBuffer *entry_buffer = NULL;
#endif

void rose_search_cb(GtkWidget *entry, gpointer data)
{
	RoseWindow *rw = rose_window_get();

#if GTK == 3
	GtkWidget *dialog = GTK_WIDGET(data);
	const char *uri = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	gtk_widget_destroy(dialog);
#elif GTK == 4
	(void) data, (void) entry;
	const char *uri = strdup(gtk_entry_buffer_get_text(entry_buffer));
#endif

	gtk_window_set_focus(rw->window, last_focused);
	rose_webview_load_uri(uri);
	free((char *)uri);
}

int rose_search_open(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	last_focused = gtk_window_get_focus(rw->window);


#if GTK == 3
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
#elif GTK == 4
	entry_buffer = gtk_entry_buffer_new("", 0);
	GtkWidget *entry = gtk_entry_new_with_buffer(entry_buffer);

	GtkWindow *popup = GTK_WINDOW(gtk_window_new());
	gtk_window_set_child(popup, entry);

	g_signal_connect(entry, "activate", G_CALLBACK(rose_search_cb), NULL);
	gtk_window_present(popup);
#endif

	return 0;
}

void rose_search_lua_api(lua_State *L)
{
	rose_lua_value("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_search_open);
	lua_setfield(L, -2, "search");
}
