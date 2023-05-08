#include <gtk/gtk.h>
#include <gdk/gdk.h>

void key_press_event(GtkWindow *self, GdkEvent *ev)
{
	(void) self;

	printf("%s\n", gdk_keyval_name(ev->key.keyval));
}

int main()
{
	GtkWindow *window;

#if GTK == 3
	gtk_init(NULL, NULL);
	window = GTK_WINDOW(gtk_window_new(0));
#elif GTK == 4
	gtk_init();
	window = gtk_window_new();
	gtk_window_present(window);
#endif

	gtk_widget_show_all(GTK_WIDGET(window));
	g_signal_connect(window, "key-press-event",
					G_CALLBACK(key_press_event), NULL);


	while (1)
		g_main_context_iteration(NULL, TRUE);

}
