#include <stdio.h>
#include <stdlib.h>
#include "keyconf.h"
#include "window.h"
#include "rose.h"

#define LENGTH(x)   ((int)(sizeof(x) / sizeof(x[0])))
#define TABS		 9

#define TAB_NEXT	 +1
#define TAB_PREV	 -1


static void load_tab(RoseWindow *w, int tab);
static bool handle_key(RoseWindow *w, int key, int keyval);
static void move_tab(RoseWindow *w, int move);
static void load_uri(RoseWebview *view, const char *uri);

static const char **glob_options;


static void die(char *msg, int exit_code)
{
	fprintf(stderr, "rose: \033[91merror(%d)\033[0m: \033[1;98m%s\033[0m\n",
			exit_code, msg);
	exit(exit_code);
}

static void read_clipboard(GObject *object, GAsyncResult *res,
		gpointer webview)
{
	GdkClipboard *clipboard = GDK_CLIPBOARD(object);
	webkit_web_view_load_uri(
	WEBKIT_WEB_VIEW(webview),
	gdk_clipboard_read_text_finish(clipboard, res, NULL));
}

static gboolean key_press_callback(RoseWindow *window, guint keyval,
		guint keycode, GdkModifierType state)
{
	(void) keycode;

	for (int i = 0; i < LENGTH(keys); i++) {
		if (keys[i].modkey == state && keys[i].keycod == keyval)
			return handle_key(window, keys[i].funcid, keyval);
	}

	return GDK_EVENT_PROPAGATE;
}

static bool handle_key(RoseWindow *window, int key, int keyval)
{
	int tab = window->tab;

	switch (key) {
		case goback:
			webkit_web_view_go_back(window->webviews[tab]->webview);
			return GDK_EVENT_STOP;

		case goforward:
			webkit_web_view_go_forward(window->webviews[tab]->webview);
			return GDK_EVENT_STOP;

		case copy_url: {
			GdkDisplay *dpy = gdk_display_get_default();
			gdk_clipboard_set_text(gdk_display_get_clipboard(dpy),
				webkit_web_view_get_uri(
					window->webviews[tab]->webview));
			return GDK_EVENT_STOP;
		}

		case paste_url: {
			GdkDisplay *dpy = gdk_display_get_default();
			GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
			gdk_clipboard_read_text_async(
				clipboard, NULL, read_clipboard,
				window->webviews[tab]->webview);
			return GDK_EVENT_STOP;
		}

		case fullscreen:
			if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(
				GTK_WIDGET(window->webviews[tab]->webview)))))
					gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(
						GTK_WIDGET(window->webviews[tab]->webview))));
			else
				gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(
					GTK_WIDGET(window->webviews[tab]->webview))));
			return GDK_EVENT_STOP;

		case search: {
			int id = fork();
			if (id == 0) {
				if (glob_dpy)
					close(ConnectionNumber(glob_dpy));
				setsid();
				char xid[16];

				snprintf(xid, 16, "%i", window->xid);
				char *argument_list[] = {"/bin/sh", "-c", "dmenu_rose",
							 xid, NULL};
				execvp("/bin/sh", argument_list);
				perror(" failed");
				exit(1);
			} else {
				wait(&id);
				char *uri = (char *)getatom(AtomGo);
				webkit_web_view_load_uri(window->webviews[tab]->webview, uri);
			return GDK_EVENT_STOP;
			}

		}

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
			return GDK_EVENT_STOP;
		}

		case findnext: {
			WebKitFindController *finder =
			webkit_web_view_get_find_controller(
				window->webviews[tab]->webview);
			webkit_find_controller_search_next(finder);
			return GDK_EVENT_STOP;
		}

		case findprev: {
			WebKitFindController *finder =
			webkit_web_view_get_find_controller(
				window->webviews[tab]->webview);
			webkit_find_controller_search_previous(finder);
			return GDK_EVENT_STOP;
		}

		case zoomin:
			window->webviews[tab]->zoom += 0.1;
			webkit_web_view_set_zoom_level(
				window->webviews[tab]->webview,
				window->webviews[tab]->zoom);
			return GDK_EVENT_STOP;

		case zoomout:
			 window->webviews[tab]->zoom -= 0.1;
			 webkit_web_view_set_zoom_level(
			window->webviews[tab]->webview,
			window->webviews[tab]->zoom);
			return GDK_EVENT_STOP;

		case zoomreset:
			 window->webviews[tab]->zoom = 1;
			 webkit_web_view_set_zoom_level(
			window->webviews[tab]->webview,
			window->webviews[tab]->zoom);
			return GDK_EVENT_STOP;

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

		case up:
			 webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollBy(0,-200);", NULL, NULL, NULL);
			return GDK_EVENT_STOP;

		case down:
			 webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollBy(0,200);", NULL, NULL, NULL);
			return GDK_EVENT_STOP;

		case reload:
			 webkit_web_view_reload(window->webviews[tab]->webview);
			return GDK_EVENT_STOP;

		case reloadforce:
			 webkit_web_view_reload_bypass_cache(
			window->webviews[tab]->webview);
			return GDK_EVENT_STOP;

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
			return GDK_EVENT_STOP;
		}

		case gotop: {
			 webkit_web_view_run_javascript(
			window->webviews[tab]->webview, "window.scrollTo(0,0);",
			NULL, NULL, NULL);
			return GDK_EVENT_STOP;
		}

		case gobottom: {
			 webkit_web_view_run_javascript(
			window->webviews[tab]->webview,
			"window.scrollTo(0, document.body.scrollHeight);", NULL,
			NULL, NULL);
			return GDK_EVENT_STOP;
		}

		case tabshow: {
			gtk_notebook_set_show_tabs(
					GTK_NOTEBOOK(window->tabs),
					!gtk_notebook_get_show_tabs(GTK_NOTEBOOK(window->tabs))
			);
			return GDK_EVENT_STOP;
		}

		case tabnext:
			move_tab(window, TAB_NEXT);
			return GDK_EVENT_STOP;

		case tabprev:
			move_tab(window, TAB_PREV);
			return GDK_EVENT_STOP;

		case tabsel: {
			int npage = keyval - 0x31;
			gtk_notebook_get_n_pages(GTK_NOTEBOOK(window->tabs));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(window->tabs), npage);
			return GDK_EVENT_STOP;
		 }
	}

	return GDK_EVENT_PROPAGATE;
}

static void rose_download(char *uri)
{
	int id = fork();
	if (id == 0) {
		setsid();
		char *argv[] = {"/bin/sh", "-c", "\"aria2c \\", uri, "\"", NULL};
		execvp("/bin/sh", argv);
		perror(" failed");
		exit(1);
	}
}

static void response_reciver(WebKitDownload *download)
{
	char *uri = (char *) webkit_uri_response_get_uri(
	webkit_download_get_response(download));

	rose_download(uri);
}

static void download_callback(WebKitDownload *download)
{
	g_signal_connect(G_OBJECT(download), "notify::response",
		G_CALLBACK(response_reciver), NULL);
}

void load_changed_callback(WebKitWebView *webview, WebKitLoadEvent event,
		RoseWindow *window)
{
	if (event == WEBKIT_LOAD_FINISHED) {
		gtk_notebook_set_tab_label_text(
			GTK_NOTEBOOK(window->tabs),
			GTK_WIDGET(webview),
			webkit_web_view_get_title(webview));

		const char *uri = webkit_web_view_get_uri(webview);
		char *cookiefile = calloc(
				1, sizeof(char) * (strlen(glob_options[CACHE]) + 32) + 1
		);

		sprintf(cookiefile, "%s/history", glob_options[CACHE]);

		FILE *cookie = fopen(cookiefile, "a");
		fprintf(cookie, "%s\n", uri);
		fclose(cookie);
		free(cookiefile);
	}
}

static void web_process_terminated_callback(WebKitWebView *webview,
		WebKitWebProcessTerminationReason reason)
{
	(void) webview;

	if (reason == WEBKIT_WEB_PROCESS_CRASHED)
		die("Webkit web process crashed.", WEBKIT_WEB_PROCESS_CRASHED);

	if (reason == WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT)
		die("Webkit web exceeded memory limit.",
			WEBKIT_WEB_PROCESS_EXCEEDED_MEMORY_LIMIT);

	if (reason == WEBKIT_WEB_PROCESS_TERMINATED_BY_API)
		die("Webkit web process terminated by api.",
			WEBKIT_WEB_PROCESS_TERMINATED_BY_API);
}

RoseWebview *rose_webview_new()
{
	WebKitUserContentManager *contentmanager;
	WebKitCookieManager *cookiemanager;
	WebKitWebContext *context;
	WebKitSettings *settings;
	RoseWebview *self;
	char cookiefile[128];

	self = malloc(sizeof(RoseWebview));
	memset(self, 0, sizeof(*self));
	self->zoom = 1;

	if (!glob_options[CACHE]) {
		const char *HOME = getenv("HOME");
		char *buf = calloc(1, sizeof(char) * (strlen(HOME) + 32) + 1);
		sprintf(buf, "%s/.cache/rose/", HOME);
		glob_options[CACHE] = buf;
	}

	context = webkit_web_context_new_with_website_data_manager(
		webkit_website_data_manager_new(
			"base-cache-directory", glob_options[CACHE],
			"base-data-directory", glob_options[CACHE],
			"hsts-cache-directory", glob_options[CACHE],
			"offline-application-cache-directory", glob_options[CACHE], NULL)
	);

	webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_WEB_BROWSER);

	/* Configure cookies. */

	cookiemanager = webkit_web_context_get_cookie_manager(context);

	strcpy(cookiefile, glob_options[CACHE]);
	strcat(cookiefile, "cookies.sqlite");

	webkit_cookie_manager_set_persistent_storage(
	cookiemanager, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

	webkit_cookie_manager_set_accept_policy(cookiemanager,
		WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

	/* Configure webkit itself. */

	settings = webkit_settings_new_with_settings(
			"auto-load-images", TRUE,
			"enable-developer-extras", TRUE,
			"enable-write-console-messages-to-stdout", TRUE,
			"enable-media-stream", TRUE,
			"enable-plugins", FALSE,
			"enable-dns-prefetching", TRUE,
			"javascript-can-access-clipboard", TRUE,
			"enable-smooth-scrolling", appearance[SMOOTHSCROLL],
			NULL
	);

	contentmanager = webkit_user_content_manager_new();
	webkit_settings_set_user_agent_with_application_details(
		settings, "Rose", "0.1");

	self->webview = g_object_new(WEBKIT_TYPE_WEB_VIEW,
			"settings", settings,
			"user-content-manager", contentmanager,
			"web-context", context, NULL
	);

	self->inspector = webkit_web_view_get_inspector(self->webview);

	/* Connect used signals to the web view. */

	g_signal_connect(G_OBJECT(self->webview), "web-process-terminated",
			G_CALLBACK(web_process_terminated_callback), NULL);
	g_signal_connect(G_OBJECT(context), "download-started",
			G_CALLBACK(download_callback), NULL);

	return self;
}

int rose_window_show(RoseWindow *window, const char *url)
{
	if (url) {
		load_uri(
			window->webviews[window->tab], url);
		setatom(AtomUri, url);
	}

	if (!(appearance[WIDTH] && appearance[HEIGHT])) {
		appearance[WIDTH] = 1280;
		appearance[HEIGHT] = 720;
	}

	gtk_window_set_default_size(GTK_WINDOW(window->window), appearance[WIDTH],
			appearance[HEIGHT]);

	gtk_widget_show(window->window);

	return window->xid = gdk_x11_surface_get_xid(
			gtk_native_get_surface(GTK_NATIVE(window->window)));
}

static void destroy(RoseWindow *window)
{
	exit(0);
}

RoseWindow *rose_window_new(GtkApplication *app, const char *options[])
{
	RoseWindow *window = malloc(sizeof(RoseWindow));
	glob_options = options;
	window->tab = 0;
	window->window = gtk_application_window_new(app);
	window->webviews = calloc(TABS, sizeof(RoseWebview *));
	gtk_widget_set_has_tooltip(window->window, FALSE);
	gtk_application_set_menubar(app, FALSE);

	window->tabs = gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(window->tabs), FALSE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(window->window), TRUE);
	gtk_window_set_child(GTK_WINDOW(window->window), window->tabs);

	g_signal_connect(G_OBJECT(window->window), "destroy",
		G_CALLBACK(destroy), window);
	load_tab(window, 0);

	return window;
}

gboolean decide_policy(WebKitWebView *v,
                       WebKitPolicyDecision *decision,
                       WebKitPolicyDecisionType type)
{
	WebKitResponsePolicyDecision *r;

	switch (type)
	{
		case WEBKIT_POLICY_DECISION_TYPE_RESPONSE:
			r = WEBKIT_RESPONSE_POLICY_DECISION(decision);
			if (!webkit_response_policy_decision_is_mime_type_supported(r))
				webkit_policy_decision_download(decision);
			else
				webkit_policy_decision_use(decision);
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

char *
untildepath(const char *path)
{
		char *apath, *name, *p;
		const char *homedir;

		if (path[1] == '/' || path[1] == '\0') {
			p = (char *)&path[1];
			homedir = getenv("HOME");
		}

		apath = g_build_filename(homedir, p, NULL);
		return apath;
}

static void load_uri(RoseWebview *view, const char *uri)
{
	char *apath, *file, *pwd;

	if (g_str_has_prefix(uri, "http://")
	 || g_str_has_prefix(uri, "https://")
	 || g_str_has_prefix(uri, "file://")
	 || g_str_has_prefix(uri, "about:")) {
		webkit_web_view_load_uri(view->webview, uri);
	} else {
		file = calloc(1, sizeof(char*));
		pwd = getenv("PWD");
		sprintf(file, "file://%s/%s", pwd, uri);
		realpath(file, NULL);
		webkit_web_view_load_uri(view->webview, file);
	}
}

static void load_tab(RoseWindow *w, int tab_)
{
	RoseWebview *tab;

	/* Create or show the tab at `tab`. */
	tab = w->webviews[tab_];
	if (!w->webviews[tab_]) {
		w->webviews[tab_] = rose_webview_new();
		tab = w->webviews[tab_];

		tab->controller = gtk_event_controller_key_new();

		g_signal_connect_swapped(tab->controller, "key-pressed",
				G_CALLBACK(key_press_callback), w);
		g_signal_connect(
				tab->webview, "load-changed",
				G_CALLBACK(load_changed_callback), w);
		g_signal_connect(
				tab->webview, "web-process-crashed",
				G_CALLBACK(web_process_terminated_callback), w);

		g_signal_connect(G_OBJECT(tab->webview), "decide-policy",
			G_CALLBACK(decide_policy), NULL);

		gtk_widget_add_controller(GTK_WIDGET(tab->webview), tab->controller);

		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tabs)) != w->tab)
			die("tried to append an unordered page", 1);
		gtk_notebook_append_page(GTK_NOTEBOOK(w->tabs),
				GTK_WIDGET(w->webviews[w->tab]->webview), NULL);

		webkit_web_view_load_uri(WEBKIT_WEB_VIEW(tab->webview),
			glob_options[HOMEPAGE]);
	}
}

static void move_tab(RoseWindow *w, int move)
{
	/* If move is TAB_NEXT, try to move to the next tab. Otherwise, move should
	   be TAB_PREV and we try to move to the previous tab. */

	if ((move == TAB_PREV && w->tab <= 0)
		|| (move == TAB_NEXT && w->tab >= TABS - 1))
		return;

	w->tab += move;
	load_tab(w, w->tab);

	if (move == TAB_PREV) {
		gtk_notebook_prev_page(GTK_NOTEBOOK(w->tabs));
		return;
	}

	gtk_notebook_next_page(GTK_NOTEBOOK(w->tabs));
}
