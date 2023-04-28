#include "webview.h"

WebKitWebView *rose_webview_new(void)
{
	WebKitWebView *view = g_object_new(WEBKIT_TYPE_WEB_VIEW, NULL);
	webkit_web_view_load_uri(view, "https://duckduckgo.com");
	return view;
}
