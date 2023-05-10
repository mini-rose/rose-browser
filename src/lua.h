#pragma once

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdbool.h>

lua_State *rose_lua_state_get();
void rose_lua_add_table(char *name);
void rose_lua_table_add_field(const char* fieldpath);
bool rose_lua_value_boolean(char *fieldpath);
char *rose_lua_value_string(char *fieldpath);
void rose_lua_value(char *fieldpath);
int rose_lua_value_number(char *fieldpath);
