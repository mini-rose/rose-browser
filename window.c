#include "window.h"
#include "rose.h"
#include "config.h"
#include "webview.h"

#define LENGTH(x) ((int) (sizeof(x) / sizeof(x[0])))

struct _RoseWindow {
	GtkApplicationWindow parent_instance;

	guint xid;
	GtkWindow *window;
	WebKitWebView *webview;
	WebKitWebInspector *inspector;
	GHashTable *action_groups;
	GHashTable *action_labels;
};

enum {
	PROP_ACTIVE_CHILD
};

static float zoom = 1.0;

G_DEFINE_TYPE(RoseWindow, rose_window, GTK_TYPE_APPLICATION_WINDOW)

static void read_clipboard(GObject *object,
                            GAsyncResult *res,
													  gpointer webview)
{
	GdkClipboard *clipboard = GDK_CLIPBOARD(object);
	webkit_web_view_load_uri(
		WEBKIT_WEB_VIEW(webview),
		gdk_clipboard_read_text_finish(clipboard, res, NULL)
	);
}

static gboolean key_press_callback(RoseWindow *window,
                                   guint keyval,
																	 guint keycode,
                                   GdkModifierType state)
{
	if (!state)
		return GDK_EVENT_PROPAGATE;

	for (int i = 0; i < LENGTH(keys); i++) {
		if (keys[i].modkey == state
				&& keys[i].keycod == keyval) {
			switch (keys[i].funcid) {
				case goback:
					webkit_web_view_go_back(window->webview);
					break;
				case goforward:
					webkit_web_view_go_forward(window->webview);
					break;
				case copy_url: {
					GdkDisplay *dpy = gdk_display_get_default();
					gdk_clipboard_set_text(
							gdk_display_get_clipboard(dpy),
							webkit_web_view_get_uri(window->webview)
					);
				} break;
				case paste_url: {
					GdkDisplay *dpy = gdk_display_get_default();
					GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
					gdk_clipboard_read_text_async(clipboard, NULL, read_clipboard, window->webview);
				} break;
				case fullscreen:
					if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview)))))
						gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview))));
					else
						gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview))));
					break;
				case search: {
					int id = fork();
					if (id == 0) {
						if (dpy)
							close(ConnectionNumber(dpy));
						close(spair[0]);
						close(spair[1]);
						setsid();
						char* argument_list[] = { "/bin/sh", "-c", "surf-open", NULL};
						execvp("/bin/sh", argument_list);
						perror(" failed");
						exit(1);
					} else {
						wait(&id);
						webkit_web_view_load_uri(window->webview, getatom(AtomGo));
					}
				}
				case zoomin:
					zoom += 0.1;
					webkit_web_view_set_zoom_level(window->webview, zoom);
					break;
				case zoomout:
					zoom -= 0.1;
					webkit_web_view_set_zoom_level(window->webview, zoom);
					break;
				case inspector:
					window->inspector = webkit_web_view_get_inspector(window->webview);
					if (webkit_web_inspector_is_attached(window->inspector))
						webkit_web_inspector_close(window->inspector);
					else
						webkit_web_inspector_show(window->inspector);
					return GDK_EVENT_STOP;
					break;
				case up:
					webkit_web_view_run_javascript(window->webview, "window.scrollBy(0,-100);", NULL, NULL, NULL);
					break;
				case down:
					webkit_web_view_run_javascript(window->webview, "window.scrollBy(0,100);", NULL, NULL, NULL);
					break;
			}
		}
	}

	return GDK_EVENT_PROPAGATE;
}

static void rose_window_constructed(GObject *object)
{
	G_OBJECT_CLASS(rose_window_parent_class)->constructed(object);
}

static void rose_window_init(RoseWindow *window)
{
	window->window = GTK_WINDOW(gtk_window_new());
}

static void destroy(RoseWindow *window)
{
	exit(0);
}

guint rose_window_show(GtkApplication *app, RoseWindow *window, const char *url)
{
	GtkWidget *w = gtk_application_window_new(app);

	gtk_application_set_menubar(app, FALSE);
	GtkWidget *webview = rose_webview_new();
	gtk_widget_set_has_tooltip(webview, FALSE);
	gtk_widget_has_default(webview);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(w), TRUE);
	window->webview = WEBKIT_WEB_VIEW(webview);

	g_signal_connect(G_OBJECT(w), "destroy",
	                 G_CALLBACK(destroy), window);
	g_signal_connect(G_OBJECT(window->webview), "web-process-terminated",
			 G_CALLBACK(destroy), window);

	if (url)
		rose_webview_load_url(WEBKIT_WEB_VIEW(webview), url);

	gtk_window_set_child(GTK_WINDOW(w), GTK_WIDGET(webview));

	/* Keyboard shortcuts */
	GtkEventController *controller;
	controller = gtk_event_controller_key_new();
	g_signal_connect_swapped(controller, "key-pressed", G_CALLBACK(key_press_callback), window);
	gtk_widget_add_controller(GTK_WIDGET(w), controller);

	gtk_widget_show(w);

	return gdk_x11_surface_get_xid(gtk_native_get_surface(GTK_NATIVE(w)));
}

void rose_window_set_webview(RoseWindow *window, GtkWidget *webview)
{
	window->webview = WEBKIT_WEB_VIEW(webview);
}

static void rose_window_class_init(RoseWindowClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(class);

	object_class->constructed = rose_window_constructed;
}

RoseWindow* rose_window_new(GtkApplication *app)
{
		return g_object_new(
			GTK_TYPE_APPLICATION_WINDOW,
			"application", GTK_APPLICATION(app),
			NULL
		);
}
