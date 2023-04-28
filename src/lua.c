#include "lua.h"
#include <lua.h>

static void rose_lua_setup()
{
	rose_lua_add_table("rose");
}

void rose_lua_add_table(char *name)
{
	lua_State *L = rose_lua_state_get();

	lua_getglobal(L, name);

	if (!lua_isnil(L, -1)) {
		warn("overrides `%s` table", name);
	}

	lua_pop(L, -1);
	lua_newtable(L);
	lua_setglobal(L, name);
}

lua_State *rose_lua_state_get()
{
	static lua_State *L = NULL;

	if (L == NULL) {
		L = luaL_newstate();
		rose_lua_setup();
	}

	return L;
}
