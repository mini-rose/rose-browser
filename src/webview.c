#include "webview.h"

WebKitWebView *rose_webview_new(void)
{
	WebKitWebView *view = WEBKIT_WEB_VIEW(webkit_web_view_new());
	webkit_web_view_load_uri(view, "https://duckduckgo.com");
	return view;
}
