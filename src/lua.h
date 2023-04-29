#pragma once

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

lua_State *rose_lua_state_get();
void rose_lua_add_table(char *name);
void rose_lua_table_add_field(char *glob_var, const char* fieldpath);
