#include "window.h"
#include "client.h"
#include "lua.h"
#include "webview.h"
#include "debug.h"
#include "keymap.h"

static void rose_window_destroy_cb(GtkWindow *window, RoseWindow *rw)
{
	(void) window;
	rose_client_destroy_by_window(rw);
}

static int rose_handle_key(RoseKeymap *rk)
{
	lua_State *L = rose_lua_state_get();
	rk->func(L);
	return 1;
}

#if GTK == 3
static int rose_keypress_event(void *self, GdkEvent *e)
{
	(void) self;
	RoseKeymapList *rkl = rose_keymap_list_get();
	GdkEventKey key = e->key;

	for (int i = 0; i < rkl->n_keymaps; i++) {
		if ((int) key.state == rkl->keymaps[i]->state
		 && (int) key.keyval == rkl->keymaps[i]->keyval)
			return rose_handle_key(rkl->keymaps[i]);
	}

	return 0;
}
#elif GTK == 4
static int rose_keypress_event(void *self, int keyval, int keycode,
							   GdkModifierType state, void *controller)
{
	(void) self, (void) keycode, (void) controller;

	RoseKeymapList *rkl = rose_keymap_list_get();

	for (int i = 0; i < rkl->n_keymaps; i++) {
		if ((int) state == rkl->keymaps[i]->state
		 && (int) keyval == rkl->keymaps[i]->keyval)
			return rose_handle_key(rkl->keymaps[i]);
	}

	return 0;
}
#endif

RoseWindow *rose_window_get(void)
{
	static RoseWindow *rw = NULL;

	if (rw == NULL)
		rw = rose_window_new();

	return rw;
}

RoseWindow *rose_window_new(void)
{
	RoseWindow *rw = calloc(1, sizeof(RoseWindow));

#if GTK == 3
	rw->window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
#elif GTK == 4
	rw->window = GTK_WINDOW(gtk_window_new());
#endif

	rw->paned = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
	rw->stack = GTK_STACK(gtk_stack_new());

	// Set window size
	int width = rose_lua_value_number("rose.settings.width");
	int height = rose_lua_value_number("rose.settings.height");

	width = (width == -1) ? 800 : width;
	height = (height == -1) ? 600 : height;

	gtk_window_set_default_size(rw->window, width, height);

	// Connect signals
#if GTK == 3
	g_signal_connect(rw->window, "key-press-event",
	                 G_CALLBACK(rose_keypress_event), NULL);
#elif GTK == 4
	GtkEventController *event_controller = gtk_event_controller_key_new();
	g_signal_connect(rw->window, "key-pressed", G_CALLBACK(rose_keypress_event), NULL);
	gtk_widget_add_controller(GTK_WIDGET(rw->window), event_controller);
#endif

#if GTK == 3
	gtk_container_add(GTK_CONTAINER(rw->window), GTK_WIDGET(rw->stack));
	gtk_stack_add_named(rw->stack, GTK_WIDGET(rose_webview_new()), "0");
	gtk_widget_show_all(GTK_WIDGET(rw->window));
#elif GTK == 4
	gtk_window_set_child(rw->window, GTK_WIDGET(rw->stack));
	gtk_stack_add_child(rw->stack, GTK_WIDGET(rose_webview_new()));
	gtk_window_present(rw->window);
#endif


	g_signal_connect(rw->window, "destroy",
				     G_CALLBACK(rose_window_destroy_cb), rw);

	return rw;
}

void rose_window_fullscreen()
{
	RoseWindow *rw = rose_window_get();
	if (gtk_window_is_fullscreen(rw->window)) {
		gtk_window_unfullscreen(rw->window);
	} else {
		gtk_window_fullscreen(rw->window);
	}
}

void rose_window_minimize()
{
	RoseWindow *rw = rose_window_get();
	static bool minimalized = false;

	if (minimalized) {
		minimalized = true;
		gtk_window_minimize(rw->window);
	} else {
		gtk_window_unmaximize(rw->window);
	}
}

void rose_window_maximize()
{
	RoseWindow *rw = rose_window_get();

	if (gtk_window_is_maximized(rw->window)) {
		gtk_window_unmaximize(rw->window);
	} else {
		gtk_window_maximize(rw->window);
	}
}

void rose_window_lua_api(lua_State *L)
{
	rose_lua_table_add_field("rose", "window.toggle");
	lua_pushcfunction(L, (lua_CFunction) rose_window_fullscreen);
	lua_setfield(L, -2, "fullscreen");

	rose_lua_table_add_field("rose", "window.toggle");
	lua_pushcfunction(L, (lua_CFunction) rose_window_minimize);
	lua_setfield(L, -2, "minimize");

	rose_lua_table_add_field("rose", "window.toggle");
	lua_pushcfunction(L, (lua_CFunction) rose_window_maximize);
	lua_setfield(L, -2, "maximize");
}

void rose_window_destroy(RoseWindow *rw)
{
	g_object_unref(rw->window);
	free(rw);
}
