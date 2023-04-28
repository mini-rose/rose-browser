#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

void _debug(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("rose: \033[35mDEBUG\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
}

void warn(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("rose: \033[33mWARNING\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
}

void error(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("rose: \033[31ERROR\033[0m: ", stderr);
	vfprintf(stderr, fmt, args);
	fputc('\n', stderr);
	va_end(args);
	exit(1);
}
