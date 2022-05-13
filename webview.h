#pragma once

#include "rose.h"

#include <webkit2/webkit2.h>
#include <gdk/gdk.h>

G_BEGIN_DECLS

#define ROSE_TYPE_WEBVIEW rose_webview_get_type()

G_DECLARE_FINAL_TYPE(RoseWebView, rose_webview, ROSE, WEBVIEW, WebKitWebView)

typedef enum
{
  ROSE_WEBVIEW_NAV_BACK = 1 << 0,
  ROSE_WEBVIEW_FORWARD  = 1 << 1
} RoseWebViewNavigationFlags;

GtkWidget* rose_webview_new();
void rose_webview_load_url(WebKitWebView *webview, const char *url);

RoseWebViewNavigationFlags rose_webview_get_navigation_flags(RoseWebView *webview);

void rose_webview_go_back(RoseWebView *window);
void rose_webview_go_forward(RoseWebView *window);

const char* rose_webview_get_address(RoseWebView *webview);
