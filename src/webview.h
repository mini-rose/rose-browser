#pragma once

#include <lua.h>

#if GTK == 4
#include <webkit/webkit.h>
#elif GTK == 3
#include <webkit2/webkit2.h>
#else
# error "Unknown gtk version"
#endif

WebKitWebView *rose_webview_new(void);
void rose_webview_load_uri(const char *);
void rose_webview_lua_api(lua_State *);
