#include "tab.h"

#include "debug.h"
#include "lua.h"
#include "window.h"
#include "webview.h"
#include <lua.h>
#include <gtk/gtk.h>

int rose_tab_new(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;

	GtkBox *box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
	gtk_box_pack_start(box, GTK_WIDGET(rose_webview_new()), true, true, 0);
	gtk_stack_add_named(stack, GTK_WIDGET(box), (char[]) { rand(), 0 });
	gtk_widget_show_all(GTK_WIDGET(rw->window));
	gtk_stack_set_visible_child(stack, GTK_WIDGET(box));

	return 0;
}

int rose_tab_next(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;

	GtkWidget *visible = gtk_stack_get_visible_child(stack);

	if (visible != NULL) {
		GList *pages = gtk_container_get_children(GTK_CONTAINER(stack));
		int index = (g_list_index(pages, visible) + 1) % g_list_length(pages);
		gtk_stack_set_visible_child(stack, g_list_nth_data(pages, index));
		g_list_free(pages);
	}

	return 0;
}

int rose_tab_prev(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;

	GtkWidget *visible = gtk_stack_get_visible_child(stack);

	if (visible != NULL) {
		GList *pages = gtk_container_get_children(GTK_CONTAINER(stack));
        gint index = g_list_index(pages, visible);

		if (index == 0) {
            index = g_list_length(pages) - 1;
        } else {
            index--;
        }

		gtk_stack_set_visible_child(stack, g_list_nth_data(pages, index));

		g_list_free(pages);
	}

	return 0;
}

int rose_tab_close(lua_State *L)
{
	(void) L;
	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;
	GtkWidget *page = gtk_stack_get_visible_child(stack);
	info("wqfgw");
	gtk_container_remove(GTK_CONTAINER(stack), page);
	return 0;
}

void rose_tab_lua_api(lua_State *L)
{
	rose_lua_table_add_field("rose.tab");
	lua_pushcfunction(L, rose_tab_new);
	lua_setfield(L, -2, "new");

	rose_lua_value("rose.tab");
	lua_pushcfunction(L, rose_tab_close);
	lua_setfield(L, -2, "close");

	rose_lua_value("rose.tab");
	lua_pushcfunction(L, rose_tab_next);
	lua_setfield(L, -2, "next");

	rose_lua_value("rose.tab");
	lua_pushcfunction(L, rose_tab_prev);
	lua_setfield(L, -2, "prev");
}
