#include "window.h"

#include <stdio.h>

#define LENGTH(x) ((int)(sizeof(x) / sizeof(x[0])))

enum { PROP_ACTIVE_CHILD };

static void die(char *msg, int exit_code) {
    puts(msg);
    exit(exit_code);
}

static void read_clipboard(GObject *object, GAsyncResult *res,
			   gpointer webview) {
    GdkClipboard *clipboard = GDK_CLIPBOARD(object);
    webkit_web_view_load_uri(
	WEBKIT_WEB_VIEW(webview),
	gdk_clipboard_read_text_finish(clipboard, res, NULL));
}

static gboolean key_press_callback(RoseWindow *window, guint keyval,
				   guint keycode, GdkModifierType state) {
    (void)keycode;

    unsigned tab = window->tab;

    if (!state && keycode != GDK_KEY_F11) return GDK_EVENT_PROPAGATE;

    for (int i = 0; i < LENGTH(keys); i++) {
	if (keys[i].modkey == state && keys[i].keycod == keyval) {
	    switch (keys[i].funcid) {
		case goback:
		    webkit_web_view_go_back(window->webviews[tab]->webview);
		    break;

		case goforward:
		    webkit_web_view_go_forward(window->webviews[tab]->webview);
		    break;

		case copy_url: {
		    GdkDisplay *dpy = gdk_display_get_default();
		    gdk_clipboard_set_text(gdk_display_get_clipboard(dpy),
					   webkit_web_view_get_uri(
					       window->webviews[tab]->webview));
		} break;

		case paste_url: {
		    GdkDisplay *dpy = gdk_display_get_default();
		    GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
		    gdk_clipboard_read_text_async(
			clipboard, NULL, read_clipboard,
			window->webviews[tab]->webview);
		} break;

		case fullscreen:
		    if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(
			    GTK_WIDGET(window->webviews[tab]->webview)))))
			gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(
			    GTK_WIDGET(window->webviews[tab]->webview))));
		    else
			gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(
			    GTK_WIDGET(window->webviews[tab]->webview))));
		    break;

		case search: {
		    int id = fork();
		    if (id == 0) {
			if (glob_dpy) close(ConnectionNumber(glob_dpy));
			setsid();
			char *argument_list[] = {"/bin/sh", "-c", "dmenu_rose",
						 NULL};
			execvp("/bin/sh", argument_list);
			perror(" failed");
			exit(1);
		    } else {
			wait(&id);
			char *uri;
			if (strcmp((uri = (char *)getatom(AtomGo)), ""))
			    webkit_web_view_load_uri(
				window->webviews[tab]->webview, uri);
		    }
		} break;

		case find: {
		    int id = fork();
		    if (id == 0) {
			if (glob_dpy) close(ConnectionNumber(glob_dpy));
			setsid();
			char *argument_list[] = {"/bin/sh", "-c",
						 "dmenu_rose\tfind", NULL};
			execvp("/bin/sh", argument_list);
			perror(" failed");
			exit(1);
		    } else {
			wait(&id);
			WebKitFindController *finder =
			    webkit_web_view_get_find_controller(
				window->webviews[tab]->webview);
			webkit_find_controller_search(
			    finder, getatom(AtomFind),
			    WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE |
				WEBKIT_FIND_OPTIONS_WRAP_AROUND,
			    G_MAXUINT);
		    }
		} break;

		case findnext: {
		    WebKitFindController *finder =
			webkit_web_view_get_find_controller(
			    window->webviews[tab]->webview);
		    webkit_find_controller_search_next(finder);
		} break;

		case findprev: {
		    WebKitFindController *finder =
			webkit_web_view_get_find_controller(
			    window->webviews[tab]->webview);
		    webkit_find_controller_search_previous(finder);
		} break;

		case zoomin:
		    window->webviews[tab]->zoom += 0.1;
		    webkit_web_view_set_zoom_level(
			window->webviews[tab]->webview,
			window->webviews[tab]->zoom);
		    break;

		case zoomout:
		    window->webviews[tab]->zoom -= 0.1;
		    webkit_web_view_set_zoom_level(
			window->webviews[tab]->webview,
			window->webviews[tab]->zoom);
		    break;

		case zoomreset:
		    window->webviews[tab]->zoom = 1;
		    webkit_web_view_set_zoom_level(
			window->webviews[tab]->webview,
			window->webviews[tab]->zoom);
		    break;

		case inspector:
		    window->webviews[tab]->inspector =
			webkit_web_view_get_inspector(
			    window->webviews[tab]->webview);
		    if (webkit_web_inspector_is_attached(
			    window->webviews[tab]->inspector))
			webkit_web_inspector_close(
			    window->webviews[tab]->inspector);
		    else
			webkit_web_inspector_show(
			    window->webviews[tab]->inspector);
		    return GDK_EVENT_STOP;
		    break;

		case up:
		    webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollBy(0,-200);", NULL, NULL, NULL);
		    break;

		case down:
		    webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollBy(0,200);", NULL, NULL, NULL);
		    break;

		case reload:
		    webkit_web_view_reload(window->webviews[tab]->webview);
		    break;

		case reloadforce:
		    webkit_web_view_reload_bypass_cache(
			window->webviews[tab]->webview);
		    break;

		case history: {
		    int id = fork();
		    if (id == 0) {
			setsid();
			char *argument_list[] = {"/bin/sh", "-c",
						 "dmenu_rose\thistory", NULL};
			execvp("/bin/sh", argument_list);
			perror(" failed");
			exit(1);
		    } else {
			wait(&id);
			char *uri;
			if (strcmp((uri = (char *)getatom(AtomGo)), ""))
			    webkit_web_view_load_uri(
				window->webviews[tab]->webview, uri);
		    }
		} break;

		case gotop: {
		    webkit_web_view_run_javascript(
			window->webviews[tab]->webview, "window.scrollTo(0,0);",
			NULL, NULL, NULL);
		} break;

		case gobottom: {
		    webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollTo(0, document.body.scrollHeight);", NULL,
			NULL, NULL);
		} break;
	    }
	}
    }

    return GDK_EVENT_PROPAGATE;
}

static void destroy(RoseWindow *window) {
    free(window->webviews);
    window->webviews = NULL;
    window = NULL;
    exit(0);
}

static void rose_download(char *uri) {
    int id = fork();
    if (id == 0) {
	setsid();
	char *argv[] = {"/bin/sh", "-c", "\"aria2c \\", uri, "\"", NULL};
	execvp("/bin/sh", argv);
	perror(" failed");
	exit(1);
    }
}

static void response_reciver(WebKitDownload *download) {
    char *uri = (char *)webkit_uri_response_get_uri(
	webkit_download_get_response(download));

    rose_download(uri);
}

static void download_callback(WebKitDownload *download) {
    g_signal_connect(G_OBJECT(download), "notify::response",
		     G_CALLBACK(response_reciver), NULL);
}

void load_changed_callback(WebKitWebView *webview, WebKitLoadEvent event) {
    if (event == WEBKIT_LOAD_FINISHED) {
	const char *uri = webkit_web_view_get_uri(webview);
	char *cookiefile =
	    calloc(1, sizeof(char) * (strlen(options[CACHE]) + 32) + 1);
	sprintf(cookiefile, "%s/history", options[CACHE]);

	FILE *cookie = fopen(cookiefile, "a");

	fprintf(cookie, "%s\n", uri);
	fclose(cookie);
	free(cookiefile);
    }
}

static void web_process_terminated_callback(
    WebKitWebView *webview, WebKitWebProcessTerminationReason reason) {
    (void)webview;

    if (reason == WEBKIT_WEB_PROCESS_CRASHED)
	die("Webkit web process crached.", WEBKIT_WEB_PROCESS_CRASHED);

    if (reason == WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT)
	die("Webkit web exceeded memory limit.",
	    WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT);

    if (reason == WEBKIT_WEB_PROCESS_TERMINATED_BY_API)
	die("Webkit web process terminated by api.",
	    WEBKIT_WEB_PROCESS_TERMINATED_BY_API);
}

RoseWebview *rose_webview_new() {
    RoseWebview *self = malloc(sizeof(RoseWebview));

    self->zoom = 1;

    WebKitSettings *settings = webkit_settings_new_with_settings(
	"auto-load-images", TRUE, "enable-developer-extras", TRUE,
	"enable-media-stream", TRUE, "enable-plugins", FALSE,
	"enable-dns-prefetching", TRUE, "javascript-can-access-clipboard", TRUE,
	NULL);

    if (!options[CACHE]) {
	const char *HOME = getenv("HOME");
	char *buf = calloc(1, sizeof(char) * (strlen(HOME) + 32) + 1);
	sprintf(buf, "%s/.cache/rose/", HOME);
	options[CACHE] = buf;
    }

    WebKitWebContext *context =
	webkit_web_context_new_with_website_data_manager(
	    webkit_website_data_manager_new(
		"base-cache-directory", options[CACHE], "base-data-directory",
		options[CACHE], NULL));

    webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_WEB_BROWSER);

    WebKitUserContentManager *contentmanager =
	webkit_user_content_manager_new();

    WebKitCookieManager *cookiemanager =
	webkit_web_context_get_cookie_manager(context);

    char cookiefile[128];

    strcpy(cookiefile, options[CACHE]);
    strcat(cookiefile, "cookies");

    webkit_cookie_manager_set_persistent_storage(
	cookiemanager, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

    webkit_cookie_manager_set_accept_policy(cookiemanager,
					    WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    self->webview = g_object_new(WEBKIT_TYPE_WEB_VIEW, "settings", settings,
				 "user-content-manager", contentmanager,
				 "web-context", context, NULL);

    self->inspector = webkit_web_view_get_inspector(self->webview);

    g_signal_connect(G_OBJECT(self->webview), "web-process-terminated",
		     G_CALLBACK(web_process_terminated_callback), NULL);

    g_signal_connect(G_OBJECT(context), "download-started",
		     G_CALLBACK(download_callback), NULL);
    return self;
}

void rose_window_show(RoseWindow *window, const char *url) {
    if (url) {
	webkit_web_view_load_uri(
	    WEBKIT_WEB_VIEW(window->webviews[window->tab]->webview), url);
	setatom(AtomUri, url);
    }

    if (!(appearance[WIDTH] && appearance[HEIGHT])) {
	appearance[WIDTH] = 1280;
	appearance[HEIGHT] = 720;
    }

    gtk_window_set_default_size(GTK_WINDOW(window->window), appearance[WIDTH],
				appearance[HEIGHT]);

    gtk_widget_show(window->window);

    window->xid = gdk_x11_surface_get_xid(
	gtk_native_get_surface(GTK_NATIVE(window->window)));
}

RoseWindow *rose_window_new(GtkApplication *app) {
    RoseWindow *window = malloc(sizeof(RoseWindow));
    window->tab = 0;
    window->window = gtk_application_window_new(app);
    window->webviews = malloc(9 * sizeof(RoseWebview));
    window->webviews[window->tab] = rose_webview_new();
    gtk_widget_set_has_tooltip(window->window, FALSE);
    gtk_application_set_menubar(app, FALSE);

    /* Keyboard shortcuts */
    GtkEventController *controller;
    controller = gtk_event_controller_key_new();

    g_signal_connect_swapped(controller, "key-pressed",
			     G_CALLBACK(key_press_callback), window);

    gtk_widget_add_controller(
	GTK_WIDGET(window->webviews[window->tab]->webview), controller);

    gtk_window_set_child(GTK_WINDOW(window->window),
			 GTK_WIDGET(window->webviews[window->tab]->webview));

    g_signal_connect(G_OBJECT(window->window), "destroy", G_CALLBACK(destroy),
		     NULL);

    return window;
}
