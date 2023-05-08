#pragma once

#include <lua.h>

typedef struct
{
	int state;
	int keyval;
	lua_CFunction func;
} RoseKeymap;

typedef struct
{
	RoseKeymap **keymaps;
	int n_keymaps;
} RoseKeymapList;

void rose_keymap_set(struct lua_State *L);
RoseKeymapList *rose_keymap_list_get(void);
void rose_keymap_lua_api(lua_State *L);
