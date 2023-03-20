/*
 * Copyright (c) 2022 mini-rose
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and any associated documentation
 * files to modify, copy, merge, publish, distribute and/or
 * sublicense copies of this sotware for their own use.
 * This code does not come with any warranty.
 *
 * Author: fenze <contact@fenze.dev>
 */

#include "config.h"

static int entry_mode;
static GtkWindow *window;
static GtkHeaderBar *bar;
static GtkEntryBuffer *search_buf;
static GtkEntry *search;

WebKitWebView *webview_new()
{
	char *style;
	WebKitSettings *settings;
	WebKitWebContext *web_context;
	WebKitCookieManager *cookiemanager;
	WebKitUserContentManager *contentmanager;

	settings = webkit_settings_new_with_settings(WEBKIT, NULL);
	web_context = webkit_web_context_new_with_website_data_manager(
	    webkit_website_data_manager_new(CACHE));
	contentmanager = webkit_user_content_manager_new();
	cookiemanager = webkit_web_context_get_cookie_manager(web_context);

	webkit_cookie_manager_set_persistent_storage(cookiemanager, CACHE_DIR "/cookies.sqlite",
						     WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

	webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

	webkit_web_context_set_process_model(web_context,
					     WEBKIT_PROCESS_MODEL_MULTIPLE_SECONDARY_PROCESSES);

	if (g_file_get_contents("~/.config/rose/style.css", &style, NULL, NULL))
		webkit_user_content_manager_add_style_sheet(
		    contentmanager,
		    webkit_user_style_sheet_new(style, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
						WEBKIT_USER_STYLE_LEVEL_USER, NULL, NULL));

	return g_object_new(WEBKIT_TYPE_WEB_VIEW, "settings", settings, "web-context", web_context,
			    "user-content-manager", contentmanager, NULL);
}

void load_uri(WebKitWebView *view, const char *uri)
{
	if (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") ||
	    g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:")) {
		webkit_web_view_load_uri(view, uri);
	} else {
		char tmp[strlen(uri) + strlen(SEARCH)];
		snprintf(tmp, sizeof(tmp), SEARCH, uri);
		webkit_web_view_load_uri(view, tmp);
	}
}

void load_changed(WebKitWebView *self, WebKitLoadEvent load_event, GtkNotebook *notebook)
{
	if (load_event == WEBKIT_LOAD_FINISHED) {
		gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(self),
						webkit_web_view_get_title(self));
		gtk_widget_hide(GTK_WIDGET(bar));
	}
}

void notebook_append(GtkNotebook *notebook, const char *uri)
{
	GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(window));
	GdkVisual *rgba_visual = gdk_screen_get_rgba_visual(screen);
	GdkRGBA rgba;

	gdk_rgba_parse(&rgba, BG_COLOR);

	WebKitWebView *view = webview_new();

	gtk_widget_set_visual(GTK_WIDGET(window), rgba_visual);
	g_signal_connect(view, "load_changed", G_CALLBACK(load_changed), notebook);

	int n = gtk_notebook_append_page(notebook, GTK_WIDGET(view), NULL);
	gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(view), true);
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_widget_hide(GTK_WIDGET(bar));
	webkit_web_view_set_background_color(view, &rgba);
	load_uri(view, (uri) ? uri : HOME);
	gtk_notebook_set_current_page(notebook, n);
}

WebKitWebView *notebook_get_webview(GtkNotebook *notebook)
{
	return WEBKIT_WEB_VIEW(
	    gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
}

void show_bar(GtkNotebook *notebook)
{
	if (entry_mode == _SEARCH) {
		const char *url = webkit_web_view_get_uri(notebook_get_webview(notebook));
		gtk_entry_set_placeholder_text(search, "Search");
		gtk_entry_buffer_set_text(search_buf, url, strlen(url));
		gtk_widget_show(GTK_WIDGET(bar));
		gtk_window_set_focus(window, GTK_WIDGET(search));
	} else {
		const char *search_text = webkit_find_controller_get_search_text(
		    webkit_web_view_get_find_controller(notebook_get_webview(notebook)));

		if (search_text != NULL)
			gtk_entry_buffer_set_text(search_buf, search_text, strlen(search_text));
		gtk_entry_set_placeholder_text(search, "Find");
		gtk_widget_show(GTK_WIDGET(bar));
		gtk_window_set_focus(window, GTK_WIDGET(search));
	}
}

int handle_key(func id, GtkNotebook *notebook)
{
	static double zoom = ZOOM;
	static bool is_fullscreen = 0;

	switch (id) {
	case goback:
		webkit_web_view_go_back(notebook_get_webview(notebook));
		break;
	case goforward:
		webkit_web_view_go_forward(notebook_get_webview(notebook));
		break;

	case refresh:
		webkit_web_view_reload(notebook_get_webview(notebook));
		break;
	case refresh_force:
		webkit_web_view_reload_bypass_cache(notebook_get_webview(notebook));
		break;

	case back_to_home:
		load_uri(notebook_get_webview(notebook), HOME);
		break;

	case zoomin:
		webkit_web_view_set_zoom_level(notebook_get_webview(notebook), (zoom += ZOOM_VAL));
		break;

	case zoomout:
		webkit_web_view_set_zoom_level(notebook_get_webview(notebook), (zoom -= ZOOM_VAL));
		break;

	case zoom_reset:
		webkit_web_view_set_zoom_level(notebook_get_webview(notebook), (zoom = ZOOM));
		break;

	case prev_tab:
		if (gtk_notebook_get_current_page(notebook) == 0) {
			gtk_notebook_set_current_page(notebook,
						      gtk_notebook_get_n_pages(notebook) - 1);
		} else {
			gtk_notebook_prev_page(notebook);
		}

		break;

	case next_tab:
		if (gtk_notebook_get_current_page(notebook) ==
		    gtk_notebook_get_n_pages(notebook) - 1) {
			notebook_append(notebook, NULL);
			gtk_notebook_set_show_tabs(notebook, true);
		} else {
			gtk_notebook_next_page(notebook);
		}
		break;

	case close_tab:
		gtk_notebook_remove_page(notebook, gtk_notebook_get_current_page(notebook));

		switch (gtk_notebook_get_n_pages(notebook)) {
		case 0:
			exit(0);
			break;
		case 1:
			gtk_notebook_set_show_tabs(notebook, false);
			break;
		}

		break;

	case toggle_fullscreen:
		if (is_fullscreen)
			gtk_window_unfullscreen(window);
		else
			gtk_window_fullscreen(window);

		is_fullscreen = ! is_fullscreen;
		break;

	case show_searchbar:
		entry_mode = _SEARCH;
		show_bar(notebook);
		break;

	case show_finder:
		entry_mode = _FIND;
		show_bar(notebook);
		break;

	case finder_next:
		webkit_find_controller_search_next(
		    webkit_web_view_get_find_controller(notebook_get_webview(notebook)));
		break;

	case finder_prev:
		webkit_find_controller_search_previous(
		    webkit_web_view_get_find_controller(notebook_get_webview(notebook)));
		break;
	}

	return 1;
}

int keypress(void *self, GdkEvent *e, GtkNotebook *notebook)
{
	(void) self;

	for (int i = 0; i < (int)(sizeof(keys) / sizeof(keys[0])); i++)
		if (e->key.keyval == keys[i].key && e->key.state == keys[i].mod)
			return handle_key(keys[i].id, notebook);

	return 0;
}

void search_activate(GtkEntry *self, GtkNotebook *notebook)
{
	(void) self;

	if (entry_mode == _SEARCH)
		load_uri(notebook_get_webview(notebook), gtk_entry_buffer_get_text(search_buf));
	else if (entry_mode == _FIND)
		webkit_find_controller_search(
		    webkit_web_view_get_find_controller(notebook_get_webview(notebook)),
		    gtk_entry_buffer_get_text(search_buf),
		    WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE | WEBKIT_FIND_OPTIONS_WRAP_AROUND,
		    G_MAXUINT);

	gtk_widget_hide(GTK_WIDGET(bar));
}

void window_init(GtkNotebook *notebook)
{
	GtkCssProvider *css = gtk_css_provider_new();

	gtk_css_provider_load_from_path(css, "/usr/share/themes/rose/style.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css),
						  800);

	gtk_entry_buffer_new("", 0);
	gtk_entry_set_alignment(search, 0.48);
	gtk_widget_set_size_request(GTK_WIDGET(search), 300, -1);
	gtk_header_bar_set_custom_title(bar, GTK_WIDGET(search));
	gtk_window_set_titlebar(window, GTK_WIDGET(bar));

	g_signal_connect(search, "activate", G_CALLBACK(search_activate), notebook);
	g_signal_connect(window, "key-press-event", G_CALLBACK(keypress), notebook);
	g_signal_connect(window, "destroy", G_CALLBACK(exit), notebook);
}

void notebook_init(GtkNotebook *notebook, const char *uri)
{
	gtk_notebook_set_show_border(notebook, false);
	gtk_notebook_set_show_tabs(notebook, false);
	notebook_append(notebook, uri);
}

void setup(GtkNotebook *notebook, const char *uri)
{
	window = GTK_WINDOW(gtk_window_new(0));
	notebook = GTK_NOTEBOOK(gtk_notebook_new());
	bar = GTK_HEADER_BAR(gtk_header_bar_new());
	search_buf = GTK_ENTRY_BUFFER(gtk_entry_buffer_new("", 0));
	search = GTK_ENTRY(gtk_entry_new_with_buffer(search_buf));

	window_init(notebook);
	notebook_init(notebook, uri);

	g_object_set(gtk_settings_get_default(), GTK, NULL);

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(notebook));
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_widget_hide(GTK_WIDGET(bar));
}

int main(int argc, char **argv)
{
	GtkNotebook *notebook;
	gtk_init(NULL, NULL);
	setup(notebook, argc > 1 ? argv[1] : NULL);
	gtk_main();
}
