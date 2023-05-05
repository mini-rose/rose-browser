#include <limits.h>
#include <stdarg.h>
#include <string.h>

char *buildpath(char *dir, ...)
{
	char path[PATH_MAX];
	char *path_part;

	va_list args;
	va_start(args, dir);

	*path = 0;
	strcat(path, dir);

	path_part = va_arg(args, char *);
	while (path_part != NULL) {
		strcat(path, "/");
		strcat(path, path_part);
		path_part = va_arg(args, char *);
	}

	va_end(args);
	return strdup(path);
}
