#pragma once

#if GTK == 4
#include <webkit/webkit.h>
#elif GTK == 3
#include <webkit2/webkit2.h>
#else
# error "Unknown gtk version"
#endif

#include "window.h"

WebKitWebView *rose_webview_new(void);

void rose_webview_reload();
void rose_webview_force_reload();
