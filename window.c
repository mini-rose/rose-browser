#include "window.h"
/* #include "gestures.h" */
#include "config.h"
#include "webview.h"

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

struct _RoseWindow {
	GtkApplicationWindow parent_instance;

	GtkWindow *window;
	WebKitWebView *webview;
	GHashTable *action_groups;
	GHashTable *action_labels;
	/* RoseGestureController *mouse_gesture_controller; */
};

enum {
	PROP_ACTIVE_CHILD
};

G_DEFINE_TYPE(RoseWindow, rose_window, GTK_TYPE_APPLICATION_WINDOW)

static void read_clipboard(GObject *object,
                            GAsyncResult *res,
													  gpointer webview)
{
	GdkDisplay *dpy = gdk_display_get_default();
	GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
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
						if (gtk_window_is_fullscreen(window->window))
							gtk_window_unfullscreen(GTK_WINDOW(window->window));
						else
							gtk_window_fullscreen(GTK_WINDOW(window->window));
						break;
				}
			}
		}

	return GDK_EVENT_PROPAGATE;
}

static void rose_window_constructed(GObject *object)
{
	RoseWindow *window;
	GAction *action;
	GActionGroup *action_group;
	GSimpleActionGroup *simple_action_group;
	guint i;
	GApplication *app;
	GtkEventController *controller;

	G_OBJECT_CLASS(rose_window_parent_class)->constructed(object);

	window = ROSE_WINDOW(window);
}

static void rose_window_init(RoseWindow *window)
{
	window->window = GTK_WINDOW(gtk_window_new());
}

void rose_window_show(RoseWindow *window)
{
	GtkWidget *w = gtk_window_new();
	window->window = GTK_WINDOW(w);
	gtk_window_set_child(GTK_WINDOW(w), GTK_WIDGET(window->webview));

	/* Keyboard shortcuts */
	GtkEventController *controller;
	controller = gtk_event_controller_key_new();
	g_signal_connect_swapped(controller, "key-pressed", G_CALLBACK(key_press_callback), window);
	gtk_widget_add_controller(GTK_WIDGET(w), controller);

	gtk_widget_show(w);
}

void rose_window_set_webview(RoseWindow *window, GtkWidget *webview)
{
	window->webview = WEBKIT_WEB_VIEW(webview);
}

static void rose_window_class_init(RoseWindowClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(class);
	GtkApplicationWindowClass *window_class = GTK_APPLICATION_WINDOW_CLASS(class);

	object_class->constructed = rose_window_constructed;

	/* g_object_class_override_property(object_class, */
                                   /* PROP_ACTIVE_CHILD, */
                                   /* "active-child"); */
}

RoseWindow* rose_window_new(GtkApplication *app)
{
		return g_object_new(
			GTK_TYPE_APPLICATION_WINDOW,
			"application", GTK_APPLICATION(app),
			NULL
		);
}
