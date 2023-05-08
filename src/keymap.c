#include "debug.h"
#include "keymap.h"
#include "lua.h"
#include <lauxlib.h>
#include <stdlib.h>
#include <gdk/gdk.h>
#include <stdbool.h>

static bool rose_keymap_equals(RoseKeymap *k1, RoseKeymap *k2)
{
	return k1->keyval == k2->keyval
	    && k1->state == k2->state;
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

	rkl->keymaps = realloc(rkl->keymaps, sizeof(void *) * (rkl->n_keymaps + 1));
	rkl->keymaps[rkl->n_keymaps++] = keymap;
	return rkl;
}

static void rose_keymap_list_remove(int index)
{
	RoseKeymapList *keymap_list = rose_keymap_list_get();

	if (index < 0 || index >= keymap_list->n_keymaps) {
        // handle index out of range error
        return;
    }

	free(keymap_list->keymaps[index]);

	memmove(keymap_list->keymaps + index,
	        keymap_list->keymaps + index + 1,
		 	sizeof(void *) * (keymap_list->n_keymaps - index - 1));
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

	keymap->keyval = gdk_keyval_from_name(key + 2);

	return 0;
}

static RoseKeymap *rose_keymap_new(const char *key)
{
	RoseKeymap *keymap = calloc(1, sizeof(RoseKeymap));

	switch (strlen(key)) {
	case 1:
		keymap->state = 0;
		keymap->keyval = gdk_keyval_from_name(key);
		break;
	default:
		if (parse_single_mod_key(keymap, key)) {
			free(keymap);
			return NULL;
		}
		break;
	}

	return keymap;
}

void rose_keymap_set(lua_State *L)
{
	const char *key = luaL_checkstring(L, 1);
	RoseKeymap *keymap = rose_keymap_new(key);

	if (!lua_isfunction(L, 2))
		error("expected lua function in rose.keymap.set");

	keymap->func = lua_tocfunction(L, 2);
	lua_pop(L, 1);

	rose_keymap_list_append(keymap);
}

RoseKeymapList *rose_keymap_list_get(void)
{
	static RoseKeymapList *rkl = NULL;

	if (rkl == NULL)
		rkl = calloc(1, sizeof(RoseKeymapList));

	return rkl;
}

void rose_keymap_del(lua_State *L)
{
	const char *key = luaL_checkstring(L, 1);
	RoseKeymapList *keymap_list = rose_keymap_list_get();
	RoseKeymap *keymap = rose_keymap_new(key);

	for (int i = 0; i < keymap_list->n_keymaps; i++) {
		if (rose_keymap_equals(keymap_list->keymaps[i], keymap)) {
			rose_keymap_list_remove(i);
			return;
		}
	}
}

void rose_keymap_lua_api(lua_State *L)
{
	rose_lua_table_add_field("rose", "keymap");
	lua_pushcfunction(L, (lua_CFunction) rose_keymap_set);
	lua_setfield(L, -2, "set");

	rose_lua_value("rose.keymap");
	lua_pushcfunction(L, (lua_CFunction) rose_keymap_del);
	lua_setfield(L, -2, "del");
}

void rose_keymap_list_destroy(RoseKeymapList *rkl)
{
	for (int i = 0; i < rkl->n_keymaps; i++)
		free(rkl->keymaps[i]);

	free(rkl);
}
