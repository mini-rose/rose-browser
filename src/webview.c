#include "webview.h"
#include "debug.h"
#include "lua.h"

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

void rose_webview_lua_api(lua_State *L)
{
	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_reload);
	lua_setfield(L, -2, "reload");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_force_reload);
	lua_setfield(L, -2, "force_reload");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_goback);
	lua_setfield(L, -2, "back");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_goforward);
	lua_setfield(L, -2, "forward");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_open);
	lua_setfield(L, -2, "open");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_zoom_reset);
	lua_setfield(L, -2, "zoom_reset");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_zoomin);
	lua_setfield(L, -2, "zoomin");

	rose_lua_table_add_field("rose.webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_zoomout);
	lua_setfield(L, -2, "zoomout");
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
	RoseWindow *rs = rose_window_get();
	GtkPaned *splitter = GTK_PANED(gtk_stack_get_visible_child(rs->stack));
	return WEBKIT_WEB_VIEW(gtk_container_get_focus_child(GTK_CONTAINER(splitter)));
}

static void load_uri(const char *uri)
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
		char tmp[lenght];
		tmp[lenght] = 0;
		snprintf(tmp, lenght - 1, search_uri, uri);
		webkit_web_view_load_uri(rose_webview_get(), tmp);
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
	load_uri(uri);
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
