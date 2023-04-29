#include "lua.h"
#include "debug.h"
#include <lua.h>
#include <stdlib.h>
#include <string.h>

static void rose_lua_setup()
{
	rose_lua_add_table("rose");
	rose_lua_table_add_field("rose", "webkit.settings");
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

void rose_lua_table_add_field(char *glob_var, const char* fieldpath)
{
	lua_State *L = rose_lua_state_get();
	lua_getglobal(L, glob_var);

    // Split the fieldpath string into parts
    char* path_copy = strdup(fieldpath);
    char* path_part = strtok(path_copy, ".");

    // Traverse the fieldpath and create nested tables as needed
    while (path_part != NULL) {
        // Push the next table onto the stack or create it if it doesn't exist
        lua_pushstring(L, path_part);
        lua_gettable(L, -2);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushstring(L, path_part);
            lua_pushvalue(L, -2);
            lua_settable(L, -4);
        }
        // Move to the next part of the fieldpath
        path_part = strtok(NULL, ".");
    }

    // Clean up
    free(path_copy);
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
