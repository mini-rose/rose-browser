#pragma once

#include <gtk/gtk.h>
#include <lua.h>

typedef struct {
	GtkWindow *window;
	GtkStack *stack;
} RoseWindow;

RoseWindow *rose_window_new(void);
RoseWindow *rose_window_get(void);
void rose_window_destroy(RoseWindow *);
void rose_window_lua_api(lua_State *L);
