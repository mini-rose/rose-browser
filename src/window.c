#include "window.h"
#include "client.h"
#include "webview.h"
#include "debug.h"

static void rose_window_destroy_cb(GtkWindow *window, RoseWindow *rw)
{
	(void) window;
	rose_client_destroy_by_window(rw);
}

RoseWindow *rose_window_new(void)
{
	debug("Initializing new window");
	RoseWindow *rw = calloc(1, sizeof(RoseWindow));

	rw->window = GTK_WINDOW(gtk_window_new());
	rw->paned = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
	rw->stack = GTK_STACK(gtk_stack_new());

	gtk_window_set_child(rw->window, GTK_WIDGET(rw->stack));
	gtk_stack_add_child(rw->stack, GTK_WIDGET(rose_webview_new()));
	debug("Opens new window");
	gtk_window_present(rw->window);

	g_signal_connect(rw->window, "destroy",
				     G_CALLBACK(rose_window_destroy_cb), rw);

	return rw;
}

void rose_window_destroy(RoseWindow *rw)
{
	g_object_unref(rw->window);
	free(rw);
}
