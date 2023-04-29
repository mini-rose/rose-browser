#include <linux/limits.h>
#include <stdarg.h>
#include <string.h>
#include "path.h"

char *buildpath(int n, ...)
{
	va_list args;
	static char buf[PATH_MAX];
	va_start(args, n);
	*buf = 0;

	for (int i = 0; i < n; i++) {
		strcat(buf, va_arg(args, char*));
		if (i + 1 != n)
			strcat(buf, "/");
	}

	va_end(args);
	return buf;
}
