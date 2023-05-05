#pragma once
#include <stdnoreturn.h>

void warn(char *fmt, ...);
noreturn void error(char *fmt, ...);

#if defined(DEBUG)
void _debug(char *fmt, ...);
# define debug(...) _debug(__VA_ARGS__)
#else
# define debug(...) (void) __VA_ARGS__
#endif
