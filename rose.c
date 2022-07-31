#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200112

#include <webkit2/webkit2.h>

#define ALTKEY 1 << 3
#define MODKEY 1 << 2
#define MODSFT 5

typedef struct {
	unsigned mod;
	int key, func;
} Key;

enum functions {
	goback, goforward, copy_url, paste_url, fullscreen, inspector, search,
	find, findnext, findprev, zoomin, zoomout, zoomreset, down, up, reload,
	reloadforce, history, gotop, gobottom, tabshow, tabnext, tabprev,
	tabsel, tabclose, hidebar };

enum appearance { HEIGHT, WIDTH, DARKMODE, SMOOTHSCROLL, ANIMATIONS };
enum privacy { CACHING, COOKIES, HISTORY };
enum options { DEFAULT, CACHE, HOMEPAGE };

#include "config.h"

typedef struct __attribute__((packed)) {
	double zoom;
	unsigned find_mode;
	WebKitWebView *webview;
	WebKitWebInspector *inspector;
	WebKitFindOptions *findopts;
	GtkEventController *controller;
} RoseWebview;

typedef struct __attribute__((packed)) {
	int            tab;
	GtkWidget      *tabview;
	GtkWidget      *window;
	GtkHeaderBar   *bar;
	GtkEntry       *searchbar;
	GtkEntryBuffer *searchbuf;
	RoseWebview    **tabs;
} RoseWindow;

static bool handle_key(RoseWindow *w, int key, int keyval);

static void read_clipboard(GObject *object, GAsyncResult *res,
                           gpointer webview)
{
	GdkClipboard *clipboard = GDK_CLIPBOARD(object);
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview),
	                         gdk_clipboard_read_text_finish(clipboard, res,
	                                                        NULL));
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
	if (has_prefix(uri)) {
		webkit_web_view_load_uri(view->webview, uri);
		return;
	}

	char tmp[254];
	snprintf(tmp, sizeof(tmp) - 1, "https://duckduckgo.com/?q=%s", uri);
	webkit_web_view_load_uri(view->webview, tmp);
}

static gboolean key_press(RoseWindow *window, int key,
                                   int keycode, unsigned mod)
{
	(void) keycode;

	for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		if (keys[i].mod == mod && keys[i].key == key)
			return handle_key(window, keys[i].func, key);

	return 0;
}

static void rose_download(const char *uri)
{
	// TODO: implement download with libcurl

	if (!fork()) {
		setsid();
		execlp("aria2c", "aria2c", uri, (char *)NULL);
		perror(" failed");
		exit(1);
	}
}

static void response_reciver(WebKitDownload *d)
{
	WebKitURIResponse *r = webkit_download_get_response(d);
	const char *uri = webkit_uri_response_get_uri(r);
	rose_download(uri);
}

static void download(WebKitDownload *d)
{
	g_signal_connect(d, "notify::response",
		(void*) response_reciver, NULL);
}

static RoseWebview *rose_webview_new(void)
{
	WebKitUserContentManager *contentmanager;
	WebKitCookieManager *cookiemanager;
	WebKitWebContext *context;
	WebKitSettings *settings;
	RoseWebview *self;
	char cookiefile[128];

	self = malloc(sizeof(RoseWebview));
	self->zoom = 1;

	if (!options[CACHE] && privacy[CACHING]) {
		const char *HOME = getenv("HOME");
		size_t bufsiz = strlen(HOME) + 33;
		char *buf = calloc(1, bufsiz);

		snprintf(buf, bufsiz - 1, "%s/.cache/rose/", HOME);
		options[CACHE] = buf;
	}

	context = privacy[CACHING]
		? webkit_web_context_new_with_website_data_manager(
			webkit_website_data_manager_new(
				"base-cache-directory", options[CACHE],
				"base-data-directory", options[CACHE], NULL))
		: webkit_web_context_new();

	if (privacy[COOKIES]) {
		cookiemanager = webkit_web_context_get_cookie_manager(context);

		strcpy(cookiefile, options[CACHE]);
		strcat(cookiefile, "cookies.sqlite");

		webkit_cookie_manager_set_persistent_storage(
		cookiemanager, cookiefile, WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

		webkit_cookie_manager_set_accept_policy(cookiemanager,
			WEBKIT_COOKIE_POLICY_ACCEPT_NO_THIRD_PARTY);
	}

	if (privacy[CACHING]) {
		webkit_web_context_set_cache_model(context, 2);
	}

	settings = webkit_settings_new_with_settings(
		"enable-developer-extras", 1,
		"enable-media-stream", 1,
		"javascript-can-access-clipboard", 1,
		"enable-smooth-scrolling", 1, NULL);

	contentmanager = webkit_user_content_manager_new();

	webkit_web_context_set_process_model(context, 1);

	self->webview = g_object_new(WEBKIT_TYPE_WEB_VIEW,
		"settings", settings,
		"user-content-manager", contentmanager,
		"web-context", context, NULL);

	self->inspector = webkit_web_view_get_inspector(self->webview);
	g_signal_connect(context, "download-started", (void*) download, NULL);

	return self;
}

static void toggle_titlebar(RoseWindow *w)
{
	GtkWidget *titlebar = gtk_window_get_titlebar(GTK_WINDOW(w->window));

	if (gtk_widget_is_visible(titlebar)) {
		gtk_widget_hide(titlebar);
		gtk_window_set_focus(GTK_WINDOW(w->window),
		                     GTK_WIDGET(w->tabs[w->tab]->webview));
		return;
	}

	if (w->tabs[w->tab]->find_mode) {
		gtk_entry_set_placeholder_text(w->searchbar, "Find");
		gtk_widget_show(titlebar);
		gtk_window_set_focus(GTK_WINDOW(w->window),
		                     GTK_WIDGET(w->searchbar));
		return;
	}

	gtk_entry_set_placeholder_text(w->searchbar, "Search");
	gtk_widget_show(titlebar);
	gtk_window_set_focus(GTK_WINDOW(w->window),
								GTK_WIDGET(w->searchbar));
}

static void append_history(const char *uri)
{
		char *cookiefile;
		FILE *cookie;
		size_t bufsiz = strlen(options[CACHE]) + 33;
		cookiefile= calloc(1, bufsiz);
		snprintf(cookiefile, bufsiz - 1, "%s/history", options[CACHE]);
		cookie = fopen(cookiefile, "a");
		fprintf(cookie, "%s\n", uri);
		fclose(cookie);
		free(cookiefile);
}

static void load_changed(WebKitWebView *v, WebKitLoadEvent e,
                                  RoseWindow *w)
{
	GtkWidget *titlebar;
	if (e != WEBKIT_LOAD_FINISHED) return;

	titlebar = gtk_window_get_titlebar(GTK_WINDOW(w->window));
	gtk_widget_hide(titlebar);
	gtk_window_set_focus(GTK_WINDOW(w->window),
	                     GTK_WIDGET(w->tabs[w->tab]->webview));
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(w->tabview), GTK_WIDGET(v),
	                                webkit_web_view_get_title(v));

	if (privacy[HISTORY]) {
		const char *uri = webkit_web_view_get_uri(v);
		append_history(uri);
	}
}

static void load_tab(RoseWindow *w, int n)
{
	RoseWebview *tab;
	GtkWidget *parent;
	if (w->tabs[n]) return;

	w->tabs[n] = rose_webview_new();
	tab = w->tabs[n];
	tab->controller = gtk_event_controller_key_new();

	g_signal_connect_swapped(tab->controller, "key-pressed",
	                         (void*) key_press, w);

	g_signal_connect(tab->webview, "load-changed",
	                 (void*) load_changed, w);

	gtk_widget_add_controller(GTK_WIDGET(tab->webview), tab->controller);

	gtk_notebook_append_page(GTK_NOTEBOOK(w->tabview),
			GTK_WIDGET(w->tabs[w->tab]->webview), NULL);

	if (appearance[ANIMATIONS]) {
		parent = gtk_widget_get_parent(GTK_WIDGET(tab->webview));
		gtk_stack_set_transition_duration(GTK_STACK(parent), 150);
		gtk_stack_set_transition_type(GTK_STACK(parent),
				GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	}

	webkit_web_view_load_uri(tab->webview, options[HOMEPAGE]);
}

static void move_tab(RoseWindow *w, int move)
{
	if ((move == -1 && w->tab == 0) || (move == 1 && w->tab == 8)) return;
	load_tab(w, w->tab += move);
	move == -1
		? gtk_notebook_prev_page(GTK_NOTEBOOK(w->tabview))
		: gtk_notebook_next_page(GTK_NOTEBOOK(w->tabview));
}

bool handle_key(RoseWindow *w, int func, int key)
{
	RoseWebview *tab = w->tabs[w->tab];

	switch (func) {
		case goback:
			webkit_web_view_go_back(tab->webview);
			break;

		case goforward:
			webkit_web_view_go_forward(tab->webview);
			break;

		case copy_url:
			gdk_clipboard_set_text(gdk_display_get_clipboard(
			                       gdk_display_get_default()),
			                       webkit_web_view_get_uri(tab->webview));
			break;

		case search:
			tab->find_mode = 0;
			toggle_titlebar(w);
			break;

		case paste_url:
			gdk_clipboard_read_text_async(gdk_display_get_clipboard(
			                              gdk_display_get_default()), NULL,
			                              read_clipboard, tab->webview);
			break;

		case fullscreen:
			if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(
				GTK_WIDGET(tab->webview)))))
					gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(
						GTK_WIDGET(tab->webview))));
			else
				gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(
					GTK_WIDGET(tab->webview))));
			break;

		case findnext:
			webkit_find_controller_search_next(
				webkit_web_view_get_find_controller(tab->webview));
			break;

		case findprev:
			webkit_find_controller_search_previous(
				webkit_web_view_get_find_controller(tab->webview));
			break;

		case find:
			tab->find_mode = 1;
			toggle_titlebar(w);
			break;

		case hidebar:
			tab->find_mode = 0;
			toggle_titlebar(w);
			break;

		case zoomin:
			webkit_web_view_set_zoom_level(tab->webview, tab->zoom += 0.1);
			break;

		case zoomout:
			webkit_web_view_set_zoom_level(tab->webview, tab->zoom -= 0.1);
			break;

		case zoomreset:
			webkit_web_view_set_zoom_level(tab->webview, tab->zoom = 1);
			break;

		case inspector:
			if (webkit_web_inspector_is_attached(tab->inspector))
				webkit_web_inspector_close(tab->inspector);
			else webkit_web_inspector_show(tab->inspector);
			break;

		case up:
		case down:
			webkit_web_view_run_javascript(tab->webview, func == up
			                               ? "self.scrollBy(0,-200)"
			                               : "self.scrollBy(0,200)", NULL,
			                               NULL, NULL);
			break;

		case reload:
			webkit_web_view_reload(tab->webview);
			break;

		case reloadforce:
			webkit_web_view_reload_bypass_cache(tab->webview);
			break;

		case gotop:
			webkit_web_view_run_javascript(tab->webview,
			                               "window.scrollTo(0,0)",
			                               NULL, NULL, NULL);
			break;

		case gobottom:
			webkit_web_view_run_javascript(tab->webview,
			                               "window.scrollTo"
			                               "(0,document.body.scrollHeight)",
			                               NULL, NULL, NULL);
			break;

		case tabshow:
			gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview),
			                           gtk_notebook_get_show_tabs(
			                           GTK_NOTEBOOK(w->tabview)));
			break;

		case tabnext:
		case tabprev:
			move_tab(w, func == tabnext ? 1 : -1);
			break;

		case tabsel: gtk_notebook_set_current_page(GTK_NOTEBOOK(w->tabview),
			                                        ((w->tab = key - 0x31)));
			break;

		case tabclose: {
			int start;

			gtk_notebook_remove_page(GTK_NOTEBOOK(w->tabview), w->tab);
			start = w->tab;

			w->tabs[w->tab] = NULL;

			w->tab = gtk_notebook_get_current_page(
					GTK_NOTEBOOK(w->tabview));

			for (int i = start; i < 8; i++) {
				if (!w->tabs[i + 1]) break;
				w->tabs[i] = w->tabs[i + 1];
				w->tabs[i + 1] = NULL;
			}

			break;
		}

		default:
			return 0;
	}

	return 1;
}

static void rose_window_show(RoseWindow *w)
{
	if (!(appearance[WIDTH] && appearance[HEIGHT]))
		appearance[WIDTH] = 1280, appearance[HEIGHT] = 720;

	gtk_window_set_default_size(GTK_WINDOW(w->window), appearance[WIDTH],
	                            appearance[HEIGHT]);
	gtk_widget_remove_css_class(w->window, "solid-csd");
	gtk_widget_hide(GTK_WIDGET(gtk_window_get_titlebar(
	                GTK_WINDOW(w->window))));
	gtk_widget_show(w->window);
}

static _Noreturn void destroy(void) { exit(0); }

static void searchbar_activate(GtkEntry *self, RoseWindow *w)
{
	(void) self;

	if (w->tabs[w->tab]->find_mode) {
		load_uri(w->tabs[w->tab], gtk_entry_buffer_get_text(w->searchbuf));
		return;
	}

	webkit_find_controller_search(webkit_web_view_get_find_controller(
											w->tabs[w->tab]->webview),
											gtk_entry_buffer_get_text(w->searchbuf),
											WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE
											| WEBKIT_FIND_OPTIONS_WRAP_AROUND,
											G_MAXUINT);
	toggle_titlebar(w);
}

static RoseWindow* rose_window_init(void)
{
	RoseWindow *w;
	setenv("GST_PLUGIN_FEATURE_RANK", "avdec_av1:NONE", 1);
	w = malloc(sizeof(RoseWindow));
	w->tab = 0;
	w->window = gtk_window_new();
	w->tabs = calloc(9, sizeof(RoseWebview *));
	w->tabview = gtk_notebook_new();
	w->bar = GTK_HEADER_BAR(gtk_header_bar_new());
	w->searchbuf = gtk_entry_buffer_new("", 0);
	w->searchbar = GTK_ENTRY(gtk_entry_new_with_buffer(w->searchbuf));

	gtk_entry_set_placeholder_text(w->searchbar, "Search");
	gtk_entry_set_icon_from_icon_name(w->searchbar, GTK_ENTRY_ICON_PRIMARY,
	                                  "edit-find-symbolic");
	gtk_widget_set_size_request(GTK_WIDGET(w->searchbar), 300, -1);
	gtk_header_bar_set_title_widget(w->bar, GTK_WIDGET(w->searchbar));
	gtk_header_bar_set_show_title_buttons(w->bar, 0);
	gtk_window_set_titlebar(GTK_WINDOW(w->window), GTK_WIDGET(w->bar));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(w->window), 1);
	gtk_window_set_child(GTK_WINDOW(w->window), w->tabview);
	gtk_widget_set_focus_child(w->window, w->tabview);

	g_signal_connect(w->searchbar, "activate",
		(void*) searchbar_activate, w);

	g_signal_connect(w->window, "destroy",
		(void*) destroy, w);

	return w;
}

static RoseWindow* rose_window_new(char *uri)
{
	RoseWindow *w = rose_window_init();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(w->tabview), 0);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(w->tabview), 0);
	load_tab(w, 0);
	load_uri(w->tabs[0], uri ? uri : options[HOMEPAGE]);
	return w;
}

static void run(char *url)
{
	GSettings *s;
	RoseWindow *window = rose_window_new(url);

	s = g_settings_new_with_path("org.gtk.gtk4.Settings.Debug",
	                             "/org/gtk/gtk4/settings/debug/");
	g_settings_set_boolean(s, "enable-inspector-keybinding", 0);

	if (appearance[DARKMODE])
		g_object_set(gtk_settings_get_default(),
		             "gtk-application-prefer-dark-theme", 1, NULL);

	if (!appearance[ANIMATIONS])
		g_object_set(gtk_settings_get_default(), "gtk-enable-animations",
		             0, NULL);

	rose_window_show(window);
}

int main(int argc, char **argv)
{
	gtk_init();
	run(argc > 1 ? argv[1] : NULL);
	while (1) g_main_context_iteration(NULL, 1);
}
