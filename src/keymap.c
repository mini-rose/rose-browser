#include "debug.h"
#include "keymap.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <stdbool.h>

static bool rose_keymap_equals(RoseKeymap *keymap, RoseKeymap *other)
{
	return !strcmp(keymap->func, other->func)
	    && keymap->keyval == other->keyval
	    && keymap->state == other->state;
}

static RoseKeymapList *rose_keymap_list_append(RoseKeymap *keymap)
{
	RoseKeymapList *rkl = rose_keymap_list_get();

	for (int i = 0; i < rkl->n_keymaps; i++) {
		if (rose_keymap_equals(keymap, rkl->keymaps[i])) {
			free(rkl->keymaps[i]);
			rkl->keymaps[i] = keymap;
			return rkl;
		}
	}

	rkl->keymaps[rkl->n_keymaps++] = keymap;
	return rkl;
}

static int parse_single_mod_key(RoseKeymap *keymap, const char *key)
{
	switch (*key) {
	case 'c':
		keymap->state = GDK_CONTROL_MASK;
		break;
	case 's':
		keymap->state = GDK_SHIFT_MASK;
		break;
	default:
		warn("cannot resolve modifier `%c` for `%s`", *key, key);
		return 1;
	}

	if (*(key + 1) != '-') {
		warn("expected `-` after modifier got `%s`", *(key + 1));
		return 1;
	}

	keymap->keyval = gdk_keyval_from_name((char[]) { *(key + 2), 0 });

	return 0;
}

void rose_keymap_set(lua_State *L)
{
	RoseKeymap *keymap = calloc(1, sizeof(RoseKeymap));

	const char *key = luaL_checkstring(L, 1);
	const char *func = luaL_checkstring(L, 2);

	switch (strlen(key)) {
	case 1:
		keymap->state = 0;
		keymap->keyval = gdk_keyval_from_name(key);
		break;
	case 3:
		if (parse_single_mod_key(keymap, key)) {
			free(keymap);
			return;
		}
		break;
	default:
		warn("wrong format of keymap: `%s`", key);
		break;
	}

	keymap->func = func;

	rose_keymap_list_append(keymap);
}

RoseKeymapList *rose_keymap_list_get(void)
{
	static RoseKeymapList *rkl = NULL;

	if (rkl == NULL)
		rkl = calloc(1, sizeof(RoseKeymapList));

	return rkl;
}

void rose_keymap_list_destroy(RoseKeymapList *rkl)
{
	for (int i = 0; i < rkl->n_keymaps; i++) {
		free(rkl->keymaps[i]);
	}

	free(rkl);
}
