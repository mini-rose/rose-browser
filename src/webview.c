#include "webview.h"
#include "debug.h"
#include "lua.h"

static WebKitSettings *rose_webview_get_settings(void)
{
	return webkit_settings_new_with_settings(
		"enable-back-forward-navigation-gestures",
		rose_lua_value_boolean("rose.webview.gestures"),
		"allow-file-access-from-file-urls",
		rose_lua_value_boolean("rose.webview.allow_file_access_from_urls"),
		"enable-developer-extras",
		rose_lua_value_boolean("rose.webview.developer_extras"),
		"enable-webgl",
		rose_lua_value_boolean("rose.webview.webgl"),
		"enable-smooth-scrolling",
		rose_lua_value_boolean("rose.webview.smooth_scrolling"),
		NULL
	);
}

void rose_webview_lua_api(lua_State *L)
{
	rose_lua_table_add_field("rose", "webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_reload);
	lua_setfield(L, -2, "reload");

	rose_lua_table_add_field("rose", "webview");
	lua_pushcfunction(L, (lua_CFunction) rose_webview_force_reload);
	lua_setfield(L, -2, "force_reload");
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

void rose_webview_reload(void)
{
	RoseWindow *rs = rose_window_get();
	webkit_web_view_reload(
		WEBKIT_WEB_VIEW(gtk_stack_get_visible_child(rs->stack))
	);
}

void rose_webview_force_reload(void)
{
	RoseWindow *rs = rose_window_get();
	webkit_web_view_reload_bypass_cache(
		WEBKIT_WEB_VIEW(gtk_stack_get_visible_child(rs->stack))
	);
}
