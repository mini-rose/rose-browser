#include "window.h"
#include "debug.h"
#include "webview.h"
#include "lua.h"
#include <stdbool.h>

static void rose_split(GtkOrientation orientation)
{
	RoseWindow *rw = rose_window_get();
	GtkWidget *focus = gtk_window_get_focus(rw->window);

	if (focus == NULL || GTK_IS_WINDOW(focus))
		return;

	GtkBox *outside = GTK_BOX(gtk_widget_get_parent(focus));

	if (outside == NULL)
		return;

	gtk_orientable_set_orientation(GTK_ORIENTABLE(outside), orientation);

	GtkBox *inside = GTK_BOX(gtk_box_new(orientation, 0));
#if GTK == 3
	gtk_box_pack_start(inside, GTK_WIDGET(rose_webview_new()), true, true, 0);
	gtk_box_pack_start(outside, GTK_WIDGET(inside), true, true, 0);
#elif GTK == 4
	gtk_box_append(inside, GTK_WIDGET(rose_webview_new()));
	gtk_box_append(outside, GTK_WIDGET(inside));
#endif

#if GTK == 3
	gtk_widget_show_all(GTK_WIDGET(rw->window));
#endif
}

void rose_window_vsplit(lua_State *L)
{
	(void) L;
	rose_split(GTK_ORIENTATION_HORIZONTAL);
}

void rose_window_hsplit(lua_State *L)
{
	(void) L;
	rose_split(GTK_ORIENTATION_VERTICAL);
}

void rose_window_split_close(lua_State *L)
{
	(void) L;

	RoseWindow *rw = rose_window_get();
	GtkWidget *focused = gtk_window_get_focus(rw->window);

	GtkWidget *layout_manager = gtk_widget_get_parent(focused);

	while (layout_manager && !GTK_IS_BOX(layout_manager))
		layout_manager = gtk_widget_get_parent(layout_manager);

	GtkWidget *main_box = gtk_widget_get_parent(focused);
	GtkWidget *buf = main_box;
	while (1) {
		if (buf == NULL)
			break;

		if (GTK_IS_BOX(buf))
			main_box = buf;

		buf = gtk_widget_get_parent(buf);
	}

#if GTK == 3
	GList *main_box_children = gtk_container_get_children(GTK_CONTAINER(main_box));
	int main_box_n_child = g_list_length(main_box_children);

	if (layout_manager != NULL && main_box_n_child > 1)
		gtk_container_remove(GTK_CONTAINER(layout_manager), focused);

	GList *layout_children = gtk_container_get_children(GTK_CONTAINER(layout_manager));
	int layout_n_child = g_list_length(layout_children);

	if (layout_n_child == 0) {
		gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(layout_manager)), layout_manager);
	}

	g_list_free(main_box_children);
#elif GTK == 4
	error("not implemented yet.");
#endif
}
