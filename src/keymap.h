#pragma once

#include <lua.h>

typedef struct
{
	int state;
	int keyval;
	const char *str;
	const char *func;
} RoseKeymap;

typedef struct
{
	RoseKeymap **keymaps;
	int n_keymaps;
} RoseKeymapList;

void rose_keymap_set(lua_State *L);
RoseKeymapList *rose_keymap_list_get(void);
