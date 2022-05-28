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

static bool handle_key(RoseWindow *w, int key, int keyval)
{
	int tab = w->tab;

	switch (key) {
		case goback:
			webkit_web_view_go_back(w->tabs[tab]->webview);
			return GDK_EVENT_STOP;

		case goforward:
			webkit_web_view_go_forward(w->tabs[tab]->webview);
			return GDK_EVENT_STOP;

		case copy_url: {
			GdkDisplay *dpy = gdk_display_get_default();
			gdk_clipboard_set_text(gdk_display_get_clipboard(dpy),
				webkit_web_view_get_uri(
					w->tabs[tab]->webview));
			return GDK_EVENT_STOP;
		}

		case paste_url: {
			GdkDisplay *dpy = gdk_display_get_default();
			GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
			gdk_clipboard_read_text_async(
				clipboard, NULL, read_clipboard,
				w->tabs[tab]->webview);
			return GDK_EVENT_STOP;
		}

		case fullscreen:
			if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(
				GTK_WIDGET(w->tabs[tab]->webview)))))
					gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(
						GTK_WIDGET(w->tabs[tab]->webview))));
			else
				gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(
					GTK_WIDGET(w->tabs[tab]->webview))));
			return GDK_EVENT_STOP;

		case search: {
			int id = fork();
			if (id == 0) {
				if (glob_dpy)
					close(ConnectionNumber(glob_dpy));
				setsid();
				char xid[16];

				snprintf(xid, 16, "%i", w->xid);
				char *argument_list[] = {"/bin/sh", "-c", "dmenu_rose",
							 xid, NULL};
				execvp("/bin/sh", argument_list);
				perror(" failed");
				exit(1);
			} else {
				wait(&id);
				char *uri = (char *)getatom(AtomGo);
				webkit_web_view_load_uri(w->tabs[tab]->webview, uri);
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
						w->tabs[tab]->webview);
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
				w->tabs[tab]->webview);
			webkit_find_controller_search_next(finder);
			return GDK_EVENT_STOP;
		}

		case findprev: {
			WebKitFindController *finder =
			webkit_web_view_get_find_controller(
				w->tabs[tab]->webview);
			webkit_find_controller_search_previous(finder);
			return GDK_EVENT_STOP;
		}

		case zoomin:
			webkit_web_view_set_zoom_level(
				w->tabs[tab]->webview,
				w->tabs[tab]->zoom += 0.1);
			return GDK_EVENT_STOP;

		case zoomout:
			 webkit_web_view_set_zoom_level(
			w->tabs[tab]->webview,
			w->tabs[tab]->zoom -= 0.1);
			return GDK_EVENT_STOP;

		case zoomreset:
			 webkit_web_view_set_zoom_level(
			w->tabs[tab]->webview,
			w->tabs[tab]->zoom = 1);
			return GDK_EVENT_STOP;

		case inspector:
			 w->tabs[tab]->inspector =
			webkit_web_view_get_inspector(
				 w->tabs[tab]->webview);
			 if (webkit_web_inspector_is_attached(
				 w->tabs[tab]->inspector))
			webkit_web_inspector_close(
				 w->tabs[tab]->inspector);
			 else
			webkit_web_inspector_show(
				 w->tabs[tab]->inspector);
			 return GDK_EVENT_STOP;

		case up:
			webkit_web_view_run_javascript(
			w->tabs[tab]->webview,
			"window.scrollBy(0,-200);", NULL, NULL, NULL);
			return GDK_EVENT_STOP;

		case down:
			 webkit_web_view_run_javascript(
			w->tabs[tab]->webview,
			"window.scrollBy(0,200);", NULL, NULL, NULL);
			return GDK_EVENT_STOP;

		case reload:
			 webkit_web_view_reload(w->tabs[tab]->webview);
			return GDK_EVENT_STOP;

		case reloadforce:
			 webkit_web_view_reload_bypass_cache(
			w->tabs[tab]->webview);
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
					w->tabs[tab]->webview, uri);
			 }
			return GDK_EVENT_STOP;
		}

		case gotop: {
			 webkit_web_view_run_javascript(
			w->tabs[tab]->webview, "window.scrollTo(0,0);",
			NULL, NULL, NULL);
			return GDK_EVENT_STOP;
		}

		case gobottom: {
			 webkit_web_view_run_javascript(
			w->tabs[tab]->webview,
			"window.scrollTo(0, document.body.scrollHeight);", NULL,
			NULL, NULL);
			return GDK_EVENT_STOP;
		}

		case tabshow: {
			gtk_notebook_set_show_tabs(
					GTK_NOTEBOOK(w->tabview),
					gtk_notebook_get_show_tabs(GTK_NOTEBOOK(w->tabview))
			);
			return GDK_EVENT_STOP;
		}

		case tabnext: {
			GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(
					w->tabs[w->tab]->webview));
			if (!gtk_stack_get_transition_running(GTK_STACK(parent)))
				move_tab(w, TAB_NEXT);
			return GDK_EVENT_STOP;
		}

		case tabprev: {
			GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(
					w->tabs[w->tab]->webview));
			if (!gtk_stack_get_transition_running(GTK_STACK(parent)))
				move_tab(w, TAB_PREV);
			return GDK_EVENT_STOP;
		}

		case tabsel: {
			int npage = keyval - 0x31;
			gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tabview));
			gtk_notebook_set_current_page(GTK_NOTEBOOK(w->tabview), npage);
			w->tab = npage;
			return GDK_EVENT_STOP;
		 }

		case tabclose:
			gtk_notebook_remove_page(GTK_NOTEBOOK(w->tabview), w->tab);
			if (!gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tabview)))
				exit(0);

			int start = w->tab;
			w->tabs[w->tab] = NULL;

			w->tab = gtk_notebook_get_current_page(
					GTK_NOTEBOOK(w->tabview));

			for (int i = start; i < TABS - 1; i++) {
				if (!w->tabs[i + 1])
					break;
				w->tabs[i] = w->tabs[i + 1];
				w->tabs[i + 1] = NULL;
			}

			return GDK_EVENT_STOP;
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

static void response_reciver(WebKitDownload *d)
{
	char *uri = (char *) webkit_uri_response_get_uri(
	webkit_download_get_response(d));
	rose_download(uri);
}

static void download_callback(WebKitDownload *download)
{
	g_signal_connect(G_OBJECT(download), "notify::response",
		G_CALLBACK(response_reciver), NULL);
}

void load_changed_callback(WebKitWebView *v, WebKitLoadEvent e,
		RoseWindow *w)
{
	if (e != WEBKIT_LOAD_FINISHED) return;

	gtk_notebook_set_tab_label_text(
		GTK_NOTEBOOK(w->tabview),
		GTK_WIDGET(v),
		webkit_web_view_get_title(v));


	if (privacy[HISTORY]) {
		const char *uri = webkit_web_view_get_uri(v);
		char *cookiefile = calloc(
				1, sizeof(char) * (strlen(options[CACHE]) + 32) + 1
		);

		sprintf(cookiefile, "%s/history", options[CACHE]);

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

static gboolean decide_policy(WebKitWebView *v,
					   WebKitPolicyDecision *d,
					   WebKitPolicyDecisionType type)
{
	(void) v;
	WebKitResponsePolicyDecision *r;

	switch (type)
	{
		case WEBKIT_POLICY_DECISION_TYPE_RESPONSE:
			r = WEBKIT_RESPONSE_POLICY_DECISION(d);
			(webkit_response_policy_decision_is_mime_type_supported(r))
				?	webkit_policy_decision_use(d)
				: webkit_policy_decision_download(d);
			break;
		case WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION:
			break;
		case WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION:
			break;
		default:
			webkit_policy_decision_ignore(d);
   }

   return TRUE;
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

	if (!options[CACHE] && privacy[CACHING]) {
		const char *HOME = getenv("HOME");
		char *buf = calloc(1, sizeof(char) * (strlen(HOME) + 32) + 1);
		sprintf(buf, "%s/.cache/rose/", HOME);
		options[CACHE] = buf;
	}

	context = (privacy[CACHING])
		? webkit_web_context_new_with_website_data_manager(
			webkit_website_data_manager_new(
				"base-cache-directory", options[CACHE],
				"base-data-directory", options[CACHE], NULL))
		: webkit_web_context_new();

	/* Configure cookies. */

	if (privacy[COOKIES]) {
		cookiemanager = webkit_web_context_get_cookie_manager(context);

		strcpy(cookiefile, options[CACHE]);
		strcat(cookiefile, "cookies.sqlite");

		webkit_cookie_manager_set_persistent_storage(
		cookiemanager, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

		webkit_cookie_manager_set_accept_policy(cookiemanager,
			WEBKIT_COOKIE_POLICY_ACCEPT_NO_THIRD_PARTY);
	}

	/* Configure webkit itself. */

	settings = webkit_settings_new_with_settings(
			"enable-developer-extras", TRUE,
			"enable-media-stream", TRUE,
			"javascript-can-access-clipboard", TRUE,
			"hardware-acceleration-policy", WEBKIT_HARDWARE_ACCELERATION_POLICY_ALWAYS,
			"enable-smooth-scrolling", appearance[SMOOTHSCROLL],
			NULL
	);

	contentmanager = webkit_user_content_manager_new();
	webkit_settings_set_user_agent_with_application_details(
		settings, "Rose", "0.1");

	self->webview = g_object_new(WEBKIT_TYPE_WEB_VIEW,
			"settings", settings,
			"user-content-manager", contentmanager,
			"web-context", context, NULL);

	self->inspector = webkit_web_view_get_inspector(self->webview);

	/* Connect used signals to the web view. */

	g_signal_connect(G_OBJECT(self->webview), "decide-policy",
		G_CALLBACK(decide_policy), NULL);

	g_signal_connect(G_OBJECT(self->webview), "web-process-terminated",
		G_CALLBACK(web_process_terminated_callback), NULL);

	g_signal_connect(G_OBJECT(context), "download-started",
		G_CALLBACK(download_callback), NULL);

	return self;
}

int rose_window_show(RoseWindow *w)
{

	if (!(appearance[WIDTH] && appearance[HEIGHT])) {
		appearance[WIDTH] = 1280;
		appearance[HEIGHT] = 720;
	}

	gtk_window_set_default_size(GTK_WINDOW(w->window), appearance[WIDTH],
			appearance[HEIGHT]);

	gtk_widget_show(w->window);

	return w->xid = gdk_x11_surface_get_xid(
			gtk_native_get_surface(GTK_NATIVE(w->window)));
}

static void destroy() { exit(0); }

static RoseWindow* rose_window_init(GtkApplication *a)
{
	RoseWindow *w = malloc(sizeof(RoseWindow));
	w->tab = 0;
	w->window = gtk_application_window_new(a);
	w->tabs = calloc(TABS, sizeof(RoseWebview *));
	w->tabview = gtk_notebook_new();

	gtk_widget_set_has_tooltip(w->window, FALSE);
	gtk_application_set_menubar(a, FALSE);

	gtk_window_set_destroy_with_parent(GTK_WINDOW(w->window), TRUE);
	gtk_window_set_child(GTK_WINDOW(w->window), w->tabview);
	gtk_widget_set_focus_child(w->window, w->tabview);

	g_signal_connect(G_OBJECT(w->window), "destroy",
		G_CALLBACK(destroy), w);

	return w;
}

RoseWindow *rose_window_new(GtkApplication *a)
{
	RoseWindow *w = rose_window_init(a);

	/* Setup notebook */

	/* TODO: Fix focus when changing tab and after that provide
	 *       keybind. For now changing tab focusing tab label */
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(w->tabview), FALSE);

	/* Setup webkit */

	load_tab(w, 0);
	load_uri(w->tabs[0], options[HOMEPAGE]);

	setatom(AtomUri, options[HOMEPAGE]);

	return w;
}

char* untildepath(const char *path)
{
		char *apath, *p;
		const char *homedir;

		if (path[1] == '/' || path[1] == '\0') {
			p = (char *)&path[1];
			homedir = getenv("HOME");
		}

		apath = g_build_filename(homedir, p, NULL);
		return apath;
}

static bool has_prefix(const char *uri)
{
	return g_str_has_prefix(uri, "http://")
		|| g_str_has_prefix(uri, "https://")
	 	|| g_str_has_prefix(uri, "file://")
	 	|| g_str_has_prefix(uri, "about:");
}

static void load_uri(RoseWebview *view, const char *uri)
{
	char *file, *pwd;

	if (!has_prefix(uri)) {
		file = calloc(256, sizeof(char));
		pwd = getenv("PWD");
		sprintf(file, "file://%s/%s", pwd, uri);
		realpath(file, NULL);
		webkit_web_view_load_uri(view->webview, file);
		return;
	}

	webkit_web_view_load_uri(view->webview, uri);
}

static void load_tab(RoseWindow *w, int tab_)
{
	RoseWebview *tab;

	/* Create or show the tab at `tab`. */
	tab = w->tabs[tab_];

	if (w->tabs[tab_]) return;

	w->tabs[tab_] = rose_webview_new();

	tab = w->tabs[tab_];

	tab->controller = gtk_event_controller_key_new();

	g_signal_connect_swapped(
		tab->controller, "key-pressed",
		G_CALLBACK(key_press_callback), w);

	g_signal_connect(
		tab->webview, "load-changed",
		G_CALLBACK(load_changed_callback), w);

	g_signal_connect(
			tab->webview, "web-process-crashed",
			G_CALLBACK(web_process_terminated_callback), w);

	gtk_widget_add_controller(
		GTK_WIDGET(tab->webview), tab->controller);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tabview)) != w->tab)
		die("tried to append an unordered page", 1);

	gtk_notebook_append_page(GTK_NOTEBOOK(w->tabview),
			GTK_WIDGET(w->tabs[w->tab]->webview), NULL);

	GtkWidget *parent = gtk_widget_get_parent(GTK_WIDGET(tab->webview));

	if (appearance[ANIMATIONS]) {
		gtk_stack_set_transition_duration(GTK_STACK(parent), 150);
		gtk_stack_set_transition_type(GTK_STACK(parent),
				GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	}

	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(tab->webview),
		options[HOMEPAGE]);
}

static void move_tab(RoseWindow *w, int move)
{
	/* If move is TAB_NEXT, try to move to the next tab. Otherwise, move should
	   be TAB_PREV and we try to move to the previous tab. */

	if ((move == TAB_PREV && w->tab == 0)
		|| (move == TAB_NEXT && w->tab == TABS - 1))
		return;

	load_tab(w, w->tab += move);

	(move == TAB_PREV)
		? gtk_notebook_prev_page(GTK_NOTEBOOK(w->tabview))
		: gtk_notebook_next_page(GTK_NOTEBOOK(w->tabview));
}
