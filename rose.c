#include "rose.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Client {
	GtkWidget *window;
	WebKitWebView *webview;
} Client;

WebKitWebView* new_webview()
{
	char cookiefile[64];
	WebKitWebView *webview;
	WebKitCookieManager *cookiemanager;
	WebKitUserContentManager *contentmanager;

	WebKitSettings *settings = webkit_settings_new_with_settings(
		"auto-load-images", true,
		"enable-back-forward-navigation-gestures", true,
		"enable-developer-extras", true,
		"enable-media-stream", true,
		"hardware-acceleration-policy", WEBKIT_HARDWARE_ACCELERATION_POLICY_ALWAYS,
		"javascript-can-access-clipboard", true, NULL);

	WebKitWebContext *context = webkit_web_context_new_with_website_data_manager(
		webkit_website_data_manager_new(
		"base-cache-directory", cachedir,
		"base-data-directory", cachedir,
		NULL));

	webkit_settings_set_user_agent_with_application_details(
		settings, "Mini", "0.1");

	webkit_web_context_set_process_model(context, WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);
	webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_WEB_BROWSER);
	contentmanager = webkit_user_content_manager_new();
	cookiemanager = webkit_web_context_get_cookie_manager(context);

	strcpy(cookiefile, cachedir);
	strcat(cookiefile, "cookies");

	webkit_cookie_manager_set_persistent_storage(cookiemanager,
		cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

	webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

	return g_object_new(
			WEBKIT_TYPE_WEB_VIEW,
			"settings", settings,
			"user-content-manager", contentmanager,
			"web-context", context, NULL);
}

static gboolean winevent(Client* c,
                     guint keyval,
										 guint keycode,
										 GdkModifierType state)
{
	if (state == GDK_CONTROL_MASK) {
		switch (keycode) {
			case 43: {
				if (webkit_web_view_can_go_back(c->webview))
					webkit_web_view_go_back(c->webview);
			} break;
			case 46: {
				if (webkit_web_view_can_go_forward(c->webview))
					webkit_web_view_go_forward(c->webview);
			} break;
			case 26: {
					webkit_web_view_load_uri(c->webview, homepage);
			} break;
		}
	}

	return GDK_EVENT_PROPAGATE;
}

static void _history_append(const char *url,
                            const char* title)
{
	char filename[] = "history";
	char *path = calloc(1, sizeof(char) * (strlen(cachedir) + strlen(filename)) + 1);
	strcpy(path, cachedir);
	strcat(path, filename);

	FILE *f;

	if (!(f = fopen(path, "r"))) {
		f = fopen(path, "w");
		fclose(f);
	}

	f = fopen(path, "a");

	fprintf(f, "'%s' - %s", title, url);
	fclose(f);
}

static void history_append(WebKitWebView *webview,
                           WebKitLoadEvent event,
													 gpointer userdata)
{
	if (event == WEBKIT_LOAD_FINISHED) {
		const char *uri = webkit_web_view_get_uri(webview);
		const char *title = webkit_web_view_get_title(webview);
		_history_append(uri, title);
	}
}

static void run(GtkApplication *app)
{
	if (dark_mode)
		g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", true, NULL);

	Client *c = calloc(1, sizeof(Client));

	c->window = gtk_window_new();
	c->webview = new_webview();

	gtk_application_add_window(app, GTK_WINDOW(c->window));

	if (homepage)
		webkit_web_view_load_uri(c->webview, homepage);

	gtk_window_set_child(GTK_WINDOW(c->window), GTK_WIDGET(c->webview));

	GtkEventController *controller = gtk_event_controller_key_new();
	gtk_widget_add_controller(GTK_WIDGET(c->window), controller);
	gtk_event_controller_set_propagation_phase (controller, GTK_PHASE_CAPTURE);
	g_signal_connect_swapped(controller, "key-pressed", G_CALLBACK (winevent), c);

	g_signal_connect(c->webview, "load-changed", G_CALLBACK(history_append), NULL);

	gtk_widget_show(GTK_WIDGET(c->window));
}

int main(int argc, char **argv)
{
	GtkApplication *app = gtk_application_new("org.gtk.rose", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(run), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
}
