#include "webview.h"
#include "lua.h"

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

	return view;
}
