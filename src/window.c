#include "window.h"
#include "client.h"
#include "webview.h"

static void rose_window_destroy_cb(GtkWindow *window, RoseWindow *rw)
{
	(void) window;
	rose_client_destroy_by_window(rw);
}

RoseWindow *rose_window_new(void)
{
	RoseWindow *rw = calloc(1, sizeof(RoseWindow));

	rw->window = GTK_WINDOW(gtk_window_new());
	rw->paned = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
	rw->stack = GTK_STACK(gtk_stack_new());

	gtk_window_set_child(rw->window, GTK_WIDGET(rw->stack));
	gtk_window_present(rw->window);
	gtk_stack_add_child(rw->stack, GTK_WIDGET(rose_webview_new()));

	g_signal_connect(rw->window, "destroy",
				     G_CALLBACK(rose_window_destroy_cb), rw);

	return rw;
}

void rose_window_destroy(RoseWindow *rw)
{
	gtk_widget_set_visible(GTK_WIDGET(rw->window), false);
	g_object_unref(rw->window);
	g_object_unref(rw->paned);
	g_object_unref(rw->stack);
	free(rw);
}
