#include <gdk/x11/gdkx.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <webkit2/webkit2.h>

#define LENGTH(x)   ((int)(sizeof(x) / sizeof(x[0])))
#define TABS        9
#define TAB_NEXT   +1
#define TAB_PREV   -1

#define MODKEY GDK_CONTROL_MASK
#define ALTKEY GDK_ALT_MASK
#define MODSFT 5
#define NOMODK 0

typedef struct {
	unsigned modkey;
	unsigned keycod;
	unsigned funcid;
} Key;

enum functions {
	goback,
	goforward,
	copy_url,
	paste_url,
	fullscreen,
	inspector,
	search,
	find,
	findnext,
	findprev,
	zoomin,
	zoomout,
	zoomreset,
	down,
	up,
	reload,
	reloadforce,
	history,
	gotop,
	gobottom,
	tabshow,
	tabnext,
	tabprev,
	tabsel,
	tabclose
};

enum appearance {
	HEIGHT,
	WIDTH,
	DARKMODE,
	SMOOTHSCROLL,
	ANIMATIONS
};

enum privacy {
	CACHING,
	COOKIES,
	HISTORY
};

enum options {
	DEFAULT,
	CACHE,
	HOMEPAGE
};

#include "config.h"

typedef struct {
	double zoom;
	WebKitWebView *webview;
	WebKitWebInspector *inspector;
	WebKitFindOptions *findopts;
	GtkEventController *controller;
} RoseWebview;

typedef struct {
	short          tab;        /* current tab */
	GtkWidget      *tabview;   /* GtkNotebook with all the tabs */
	GtkWidget      *window;    /* app window */
	GtkHeaderBar   *bar;      /* header bar */
	GtkEntry       *searchbar;
	GtkEntryBuffer *searchbuf;
	RoseWebview    **tabs;     /* array of tabs */
} RoseWindow;

static void load_tab(RoseWindow *w, int tab);
static bool handle_key(RoseWindow *w, int key, int keyval);
static void move_tab(RoseWindow *w, int move);
static void load_uri(RoseWebview *view, const char *uri);

void die(char *msg, int exit_code)
{
	fprintf(stderr, "rose: \033[91merror(%d)\033[0m: \033[1;98m%s\033[0m\n",
		exit_code, msg);
	exit(exit_code);
}

void read_clipboard(GObject *object, GAsyncResult *res,
		gpointer webview)
{
	GdkClipboard *clipboard = GDK_CLIPBOARD(object);
	webkit_web_view_load_uri(
		WEBKIT_WEB_VIEW(webview),
		gdk_clipboard_read_text_finish(clipboard, res, NULL));
}

gboolean key_press_callback(RoseWindow *window, guint keyval,
		guint keycode, GdkModifierType state)
{
	(void) keycode;

	for (int i = 0; i < LENGTH(keys); i++)
		if (keys[i].modkey == state && keys[i].keycod == keyval)
			return handle_key(window, keys[i].funcid, keyval);


	return GDK_EVENT_PROPAGATE;
}

bool handle_key(RoseWindow *w, int key, int keyval)
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

		case search: {
			GtkWidget *titlebar = gtk_window_get_titlebar(GTK_WINDOW(w->window));
			if (gtk_widget_is_visible(titlebar)) {
				gtk_widget_hide(titlebar);
				gtk_window_set_focus(GTK_WINDOW(w->window),
					GTK_WIDGET(w->tabs[tab]->webview));
				gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview), FALSE);
			}
			else {
				gtk_widget_show(titlebar);
				gtk_window_set_focus(GTK_WINDOW(w->window),
						GTK_WIDGET(w->searchbar));
				gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview), TRUE);
			}
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
			gtk_notebook_get_n_pages(GTK_NOTEBOOK(w->tabview));
			gtk_notebook_set_current_page(
				GTK_NOTEBOOK(w->tabview), ((w->tab = keyval - 0x31)));
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

void rose_download(char *uri)
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

void response_reciver(WebKitDownload *d)
{
	char *uri = (char *) webkit_uri_response_get_uri(
	webkit_download_get_response(d));
	rose_download(uri);
}

void download_callback(WebKitDownload *download)
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

	GtkWidget *titlebar = gtk_window_get_titlebar(GTK_WINDOW(w->window));

	if (gtk_widget_is_visible(titlebar)) {
		gtk_widget_hide(titlebar);
	}

	gtk_window_set_focus(GTK_WINDOW(w->window),
		GTK_WIDGET(w->tabs[w->tab]->webview));

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
		"enable-smooth-scrolling", appearance[SMOOTHSCROLL],
		NULL);

	contentmanager = webkit_user_content_manager_new();
	webkit_settings_set_user_agent_with_application_details(
		settings, "Rose", "0.2");

	self->webview = g_object_new(WEBKIT_TYPE_WEB_VIEW,
		"settings", settings,
		"user-content-manager", contentmanager,
		"web-context", context, NULL);

	self->inspector = webkit_web_view_get_inspector(self->webview);

	/* Connect used signals to the web view. */

	g_signal_connect(G_OBJECT(context), "download-started",
		G_CALLBACK(download_callback), NULL);


	return self;
}

void rose_window_show(RoseWindow *w)
{

	if (!(appearance[WIDTH] && appearance[HEIGHT])) {
		appearance[WIDTH] = 1280;
		appearance[HEIGHT] = 720;
	}

	gtk_window_set_default_size(
			GTK_WINDOW(w->window),
			appearance[WIDTH], appearance[HEIGHT]);

	gtk_widget_remove_css_class(w->window, "solid-csd");
	gtk_widget_hide(GTK_WIDGET(gtk_window_get_titlebar(GTK_WINDOW(w->window))));
	gtk_widget_show(w->window);
	gtk_window_set_focus(GTK_WINDOW(w->window),
		GTK_WIDGET(w->tabs[w->tab]->webview));
}

void destroy() { exit(0); }

void searchbar_activate(GtkEntry *self, RoseWindow *w)
{
	load_uri(w->tabs[w->tab],
		gtk_entry_buffer_get_text(w->searchbuf));
}

RoseWindow* rose_window_init()
{
	setenv("GST_PLUGIN_FEATURE_RANK", "avdec_av1:NONE", 1);
	RoseWindow *w = malloc(sizeof(RoseWindow));
	w->tab = 0;
	w->window = gtk_window_new();
	w->tabs = calloc(TABS, sizeof(RoseWebview *));
	w->tabview = gtk_notebook_new();
	w->bar = GTK_HEADER_BAR(gtk_header_bar_new());

	w->searchbuf = gtk_entry_buffer_new("", 0);
	w->searchbar = GTK_ENTRY(gtk_entry_new_with_buffer(w->searchbuf));


	gtk_widget_set_size_request(GTK_WIDGET(w->searchbar), 300, -1);
	gtk_header_bar_set_title_widget(w->bar, GTK_WIDGET(w->searchbar));
	gtk_header_bar_set_show_title_buttons(w->bar, FALSE);
	gtk_window_set_titlebar(GTK_WINDOW(w->window), GTK_WIDGET(w->bar));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(w->window), TRUE);
	gtk_window_set_child(GTK_WINDOW(w->window), w->tabview);
	gtk_widget_set_focus_child(w->window, w->tabview);

	g_signal_connect(w->searchbar, "activate",
		G_CALLBACK(searchbar_activate), w);
	g_signal_connect(G_OBJECT(w->window), "destroy",
		G_CALLBACK(destroy), w);


	return w;
}

RoseWindow *rose_window_new(char *uri)
{
	RoseWindow *w = rose_window_init();

	/* Setup notebook */

	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(w->tabview), FALSE);

	/* Setup webkit */

	load_tab(w, 0);
	load_uri(w->tabs[0], (uri) ? uri : options[HOMEPAGE]);

	return w;
}

bool has_prefix(const char *uri)
{
	return g_str_has_prefix(uri, "http://")
		|| g_str_has_prefix(uri, "https://")
	 	|| g_str_has_prefix(uri, "file://")
	 	|| g_str_has_prefix(uri, "about:");
}

void load_uri(RoseWebview *view, const char *uri)
{
	char *pwd;

	if (!has_prefix(uri)) {
		char tmp[254];
		sprintf(tmp, "https://duckduckgo.com/?q=%s", uri);
		webkit_web_view_load_uri(view->webview, tmp);
		return;
	}

	webkit_web_view_load_uri(view->webview, uri);
}

void load_tab(RoseWindow *w, int tab_)
{
	RoseWebview *tab;
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

void move_tab(RoseWindow *w, int move)
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

void run(char *url)
{
	RoseWindow *window;

	/* We need to pass our instance of options, because it's a static variable
	   so each file gets its own instace. */
	window = rose_window_new(url);

	if (appearance[DARKMODE])
		g_object_set(gtk_settings_get_default(),
				"gtk-application-prefer-dark-theme", true, NULL);

	if (!appearance[ANIMATIONS])
		g_object_set(gtk_settings_get_default(), "gtk-enable-animations", false,
				NULL);

	rose_window_show(window);
}

int main(int argc, char **argv)
{
	gtk_init();

	run((argc > 1) ? argv[1] : NULL);

	while (1)
		g_main_context_iteration(NULL, TRUE);

	return 0;
}
