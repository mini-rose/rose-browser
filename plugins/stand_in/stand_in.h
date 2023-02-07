/* Why this file is needed:
 * If the plugins are disabled, 
 * their conditionals will never resolve
 * and their functionality never comes into play
 * but the compiler still wants to know their type
*/

#pragma once

#define LIBRE_N 0
#define STYLE_N 0
#define READABILITY_N 0

int libre_redirect(const char* uri, char* uri_filtered);
void str_init(char* str, int n);

int str_replace_start(const char* string, const char* target,
          const char* replacement, char* output);

void read_readability_js(char* string);

void read_style_js(char* string);

