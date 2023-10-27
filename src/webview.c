#include "webview.h"
#include "lua.h"
#include "window.h"
#include "debug.h"

void rose_webview_run_javascript(gchar *script);

static WebKitSettings *rose_webview_get_settings(void)
{
	static WebKitSettings *settings = NULL;

	if (settings == NULL) {
		settings = webkit_settings_new_with_settings(
			"enable-back-forward-navigation-gestures",
			rose_lua_value_boolean("rose.webkit.settings.gestures"),
			"allow-file-access-from-file-urls",
			rose_lua_value_boolean("rose.webkit.settings.allow_file_access_from_urls"),
			"enable-developer-extras",
			rose_lua_value_boolean("rose.webkit.settings.developer_extras"),
			"enable-webgl",
			rose_lua_value_boolean("rose.webkit.settings.webgl"),
			"enable-smooth-scrolling",
			rose_lua_value_boolean("rose.webkit.settings.smooth_scrolling"),
			NULL
		);
	}

	return settings;
}

WebKitWebView *rose_webview_new(void)
{
	WebKitWebView *view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	char *uri = rose_lua_value_string("rose.startpage");

	if (uri == NULL) {
		webkit_web_view_load_uri(view, "https://duckduckgo.com");
	} else {
		webkit_web_view_load_uri(view, uri);
		free(uri);
	}

	webkit_web_view_set_settings(view, rose_webview_get_settings());

	return view;
}

static WebKitWebView *rose_webview_get(void)
{
	RoseWindow *rw = rose_window_get();
	GtkWidget *focus = gtk_window_get_focus(rw->window);

	return WEBKIT_WEB_VIEW(focus);
}

void rose_webview_run_javascript(gchar *script)
{
	webkit_web_view_evaluate_javascript(
		rose_webview_get(),
		script, -1, NULL, NULL, NULL,
		NULL, NULL
	);
}

void rose_webview_scroll_to_bottom()
{
	rose_webview_run_javascript("window.scrollTo(0, document.body.scrollHeight);");
}

void rose_webview_scroll_to_top()
{
	rose_webview_run_javascript("window.scrollTo(0, 0);");
}

void rose_webview_scroll_down()
{
	rose_webview_run_javascript("window.scrollBy(0, 100);");
}

void rose_webview_scroll_up()
{
	rose_webview_run_javascript("window.scrollBy(0, -100);");
}

void rose_webview_scroll_left()

{
	rose_webview_run_javascript("window.scrollBy(-100, 0);");
}

void rose_webview_scroll_right()
{
	rose_webview_run_javascript("window.scrollBy(100, 0);");
}

void rose_webview_scroll_page_down()
{
	rose_webview_run_javascript("window.scrollBy(0, window.innerHeight);");
}

void rose_webview_scroll_page_up()
{
	rose_webview_run_javascript("window.scrollBy(0, -window.innerHeight);");
}

void rose_webview_scroll_page_left()
{
	rose_webview_run_javascript("window.scrollBy(-window.innerWidth, 0);");
}

void rose_webview_scroll_page_right()
{
	rose_webview_run_javascript("window.scrollBy(window.innerWidth, 0);");
}

void rose_webview_evaluate_javascript(lua_State *L)
{
	char *script = (char *) luaL_checkstring(L, 1);

	if (script == NULL) {
		return;
	}

	rose_webview_run_javascript(script);
}

void rose_webview_load_uri(const char *uri)
{
	if (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") ||
	    g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:")) {
		webkit_web_view_load_uri(rose_webview_get(), uri);
	} else {
		char *search_uri = rose_lua_value_string("rose.search");

		if (search_uri == NULL) {
			search_uri = strdup("https://duckduckgo.com/?q=%s");
		}

		int lenght = strlen(uri) + strlen(search_uri) + 1;
		char *tmp = calloc(1, lenght);
		snprintf(tmp, lenght - 1, search_uri, uri);
		webkit_web_view_load_uri(rose_webview_get(), tmp);
		free(tmp);
		free(search_uri);
	}
}

void rose_webview_reload(void)
{
	webkit_web_view_reload(rose_webview_get());
}

void rose_webview_goback(void)
{
	webkit_web_view_go_back(rose_webview_get());
}

void rose_webview_goforward(void)
{
	webkit_web_view_go_forward(rose_webview_get());
}

void rose_webview_open(lua_State *L)
{
	const char *uri = luaL_checkstring(L, 1);
	rose_webview_load_uri(uri);
}

void rose_webview_zoomin(void)
{
	double zoom = webkit_web_view_get_zoom_level(rose_webview_get());
	webkit_web_view_set_zoom_level(rose_webview_get(), zoom += 0.1);
}

void rose_webview_zoomout(void)
{
	double zoom = webkit_web_view_get_zoom_level(rose_webview_get());
	webkit_web_view_set_zoom_level(rose_webview_get(), zoom -= 0.1);
}

void rose_webview_zoom_reset(void)
{
	webkit_web_view_set_zoom_level(rose_webview_get(), 1);
}

void rose_webview_force_reload(void)
{
	webkit_web_view_reload_bypass_cache(rose_webview_get());
}

void rose_webview_lua_api(lua_State *L)
{
	struct {
		void *func;
		char *name;
	} api[] = {
		{ rose_webview_open, "open" },
		{ rose_webview_reload, "reload" },
		{ rose_webview_force_reload, "force_reload" },
		{ rose_webview_goback, "goback" },
		{ rose_webview_goforward, "goforward" },
		{ rose_webview_zoomout, "zoomout" },
		{ rose_webview_zoomin, "zoomin" },
		{ rose_webview_load_uri, "load_uri" },
		{ rose_webview_zoom_reset, "zoom_reset" },
		{ rose_webview_scroll_to_bottom, "scroll_to_bottom" },
		{ rose_webview_scroll_to_top, "scroll_to_top" },
		{ rose_webview_scroll_down, "scroll_down" },
		{ rose_webview_scroll_up, "scroll_up" },
		{ rose_webview_scroll_left, "scroll_left" },
		{ rose_webview_scroll_right, "scroll_right" },
		{ rose_webview_scroll_page_down, "scroll_page_down" },
		{ rose_webview_scroll_page_up, "scroll_page_up" },
		{ rose_webview_scroll_page_left, "scroll_page_left" },
		{ rose_webview_scroll_page_right, "scroll_page_right" },
		{ rose_webview_evaluate_javascript, "evaluate_javascript" }
	};

	for (ulong i = 0; i < sizeof(api) / sizeof(api[0]); i++) {
		rose_lua_table_add_field("rose.webview");
		lua_pushcfunction(L, (lua_CFunction) api[i].func);
		lua_setfield(L, -2, api[i].name);
	}
}
