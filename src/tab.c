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
#if GTK == 3
	gtk_box_pack_start(box, GTK_WIDGET(rose_webview_new()), true, true, 0);
#elif GTK == 4
	gtk_box_append(box, GTK_WIDGET(rose_webview_new()));
#endif
	gtk_stack_add_named(stack, GTK_WIDGET(box), (char[]) { rand(), 0 });

#if GTK == 3
	gtk_widget_show_all(GTK_WIDGET(rw->window));
#endif
	gtk_stack_set_visible_child(stack, GTK_WIDGET(box));

	return 0;
}

#if GTK == 4
gint get_index_from_list_model(GListModel *list_model, gpointer item)
{
    gint index = -1;
    guint n_items = g_list_model_get_n_items(list_model);

    for (guint i = 0; i < n_items; i++) {
        gpointer current_item = g_list_model_get_item(list_model, i);
        if (current_item == item) {
            index = i;
            break;
        }
    }

    return index;
}
#endif

int rose_tab_next(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;

	GtkWidget *visible = gtk_stack_get_visible_child(stack);

	if (visible != NULL) {
#if GTK == 3
		GList *pages = gtk_container_get_children(GTK_CONTAINER(stack));
		int index = (g_list_index(pages, visible) + 1) % g_list_length(pages);
		gtk_stack_set_visible_child(stack, g_list_nth_data(pages, index));
		g_list_free(pages);
#elif GTK == 4
		GListModel *pages = G_LIST_MODEL(gtk_stack_get_pages(stack));
		int index = (get_index_from_list_model(pages, visible) + 1) % g_list_model_get_n_items(pages);
		gtk_stack_set_visible_child(stack, g_list_model_get_item(pages, index));
#endif
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
#if GTK == 3
		GList *pages = gtk_container_get_children(GTK_CONTAINER(stack));
        gint index = g_list_index(pages, visible);
#elif GTK == 4
		GListModel *pages = G_LIST_MODEL(gtk_stack_get_pages(stack));
		int index = (get_index_from_list_model(pages, visible) + 1) % g_list_model_get_n_items(pages);
#endif

		if (index == 0) {
#if GTK == 3
            index = g_list_length(pages) - 1;
#elif GTK == 4
			index = g_list_model_get_n_items(pages) - 1;
#endif
        } else {
            index--;
        }

#if GTK == 3
		gtk_stack_set_visible_child(stack, g_list_nth_data(pages, index));
		g_list_free(pages);
#elif GTK == 4
		gtk_stack_set_visible_child(stack, g_list_model_get_item(pages, index));
#endif
	}

	return 0;
}

int rose_tab_close(lua_State *L)
{
	(void) L;
	RoseWindow *rw = rose_window_get();
	GtkStack *stack = rw->stack;
	GtkWidget *page = gtk_stack_get_visible_child(stack);
#if GTK == 3
	gtk_container_remove(GTK_CONTAINER(stack), page);
#elif GTK == 4
	gtk_stack_remove(stack, page);
#endif
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
