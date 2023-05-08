#pragma once

#include <lua.h>

#if GTK == 4
#include <webkit/webkit.h>
#elif GTK == 3
#include <webkit2/webkit2.h>
#else
# error "Unknown gtk version"
#endif

#include "window.h"

WebKitWebView *rose_webview_new(void);

// Actions
void rose_webview_open(lua_State *);
void rose_webview_reload();
void rose_webview_force_reload();
void rose_webview_goback(void);
void rose_webview_goforward(void);
void rose_webview_zoomout(void);
void rose_webview_zoomin(void);
void rose_webview_zoom_reset(void);

// Api
void rose_webview_lua_api(lua_State *);
