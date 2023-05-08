#include "window.h"
#include "client.h"
#include "lua.h"
#include "webview.h"
#include "debug.h"

static void rose_window_destroy_cb(GtkWindow *window, RoseWindow *rw)
{
	(void) window;
	rose_client_destroy_by_window(rw);
}

RoseWindow *rose_window_get(void)
{
	static RoseWindow *rw = NULL;

	if (rw == NULL)
		rw = rose_window_new();

	return rw;
}

RoseWindow *rose_window_new(void)
{
	RoseWindow *rw = calloc(1, sizeof(RoseWindow));

#if GTK == 3
	rw->window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
#elif GTK == 4
	rw->window = GTK_WINDOW(gtk_window_new());
#endif

	rw->paned = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
	rw->stack = GTK_STACK(gtk_stack_new());

	debug("Opens new window");
#if GTK == 3
	gtk_container_add(GTK_CONTAINER(rw->window), GTK_WIDGET(rw->stack));
	gtk_stack_add_named(rw->stack, GTK_WIDGET(rose_webview_new()), "0");
	gtk_widget_show_all(GTK_WIDGET(rw->window));
#elif GTK == 4
	gtk_window_set_child(rw->window, GTK_WIDGET(rw->stack));
	gtk_stack_add_child(rw->stack, GTK_WIDGET(rose_webview_new()));
	gtk_window_present(rw->window);
#endif

	g_signal_connect(rw->window, "destroy",
				     G_CALLBACK(rose_window_destroy_cb), rw);

	return rw;
}

void rose_window_destroy(RoseWindow *rw)
{
	g_object_unref(rw->window);
	free(rw);
}
