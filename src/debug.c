#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

#if defined(DEBUG)
void _debug(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("\033[1;35mdebug\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
}
#endif

void warn(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("\033[1;33mwarning\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
}

void error(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("rose: \033[1;31error\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
	exit(1);
}

void info(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("\033[1;34mnote\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
}
