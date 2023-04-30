#include "lua.h"
#include "debug.h"
#include "path.h"

#include <lualib.h>
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

static void rose_lua_value(char *fieldpath)
{
	lua_State *L = rose_lua_state_get();
	lua_pop(L, -1);

	char *path = strdup(fieldpath);
	char *path_chunk = strtok(path, ".");
	lua_getglobal(L, path_chunk);

	if (lua_isnil(L, -1)) {
		warn("cannot access `%s`, the `%s` does not exist", fieldpath, path_chunk);
		free(path);
		return;
	}

	while (path_chunk != NULL) {
		path_chunk = strtok(NULL, ".");

		if (path_chunk == NULL)
			break;

		lua_getfield(L, -1, path_chunk);

		if (lua_isnil(L, -1)) {
			warn("cannot access `%s`, the field `%s` does not exist", fieldpath, path_chunk);
			free(path);
			return;
		}
	}

	free(path);
}

bool rose_lua_value_boolean(char *fieldpath)
{
	lua_State *L = rose_lua_state_get();
	rose_lua_value(fieldpath);
	bool value = lua_toboolean(L, -1);
	lua_pop(L, -1);
	return value;
}

char *rose_lua_value_string(char *fieldpath)
{
	lua_State *L = rose_lua_state_get();

	rose_lua_value(fieldpath);

	char *value = strdup(lua_tostring(L, -1));
	lua_pop(L, -1);
	return value;
}

void rose_lua_table_add_field(char *glob_var, const char *fieldpath)
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
		luaL_openlibs(L);
		rose_lua_setup();
		luaL_dofile(L, buildpath(2, getenv("HOME"), ".config/rose/init.lua"));
	}

	return L;
}
