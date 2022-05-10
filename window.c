#include "window.h"
/* #include "gestures.h" */
#include "config.h"
#include "webview.h"

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

static gboolean key_press_callback(WebKitWebView *webview,
                                   guint keyval,
																	 guint keycode,
                                   GdkModifierType state)
{

	if (state & GDK_CONTROL_MASK) {
			switch (keycode) {
				case 43: {
					webkit_web_view_go_back(webview);
				} break;
				case 46: {
					webkit_web_view_go_forward(webview);
				} break;
				case 26: {
						rose_webview_load_url(webview, homepage);
				} break;
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
	gtk_window_set_child(GTK_WINDOW(w), GTK_WIDGET(window->webview));

	/* Keyboard shortcuts */
	GtkEventController *controller;
	controller = gtk_event_controller_key_new();
	g_signal_connect_swapped(controller, "key-pressed", G_CALLBACK(key_press_callback), window->webview);
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
