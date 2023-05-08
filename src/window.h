#pragma once

#include <gtk/gtk.h>

typedef struct {
	GtkWindow *window;
	GtkStack *stack;
	GtkPaned *paned;
} RoseWindow;

RoseWindow *rose_window_new(void);
RoseWindow *rose_window_get(void);
void rose_window_destroy(RoseWindow *);
