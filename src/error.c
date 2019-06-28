#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"
#include "common.h"

#define ERRBUFLEN_ 500

static void _pr_err(const char *file, int line, const char *func,
	     int errnum, const char *errmsg, va_list ap)
{
	const char *app_name = app_get_name();
	char buf[ERRBUFLEN_ + 1];

#ifdef DEBUG
	fprintf(stderr, "[%s:%d - %s()] ", file, line, func);
#endif
	if (!app_name)
		app_name = "this?";
	vsnprintf(buf, ERRBUFLEN_, errmsg, ap);
	if (errnum)
		fprintf(stderr, "%s: [error] [%d: %s] [%s]\n",
			app_name, errnum, strerror(errnum), buf);
	else
		fprintf(stderr, "%s: [error] [%s]\n", app_name, buf);
}

void _err(const char *file, int line, const char *func,
	  const char *errmsg, ...)
{
	int olderr = errno;

	va_list ap;
	va_start(ap, errmsg);
	_pr_err(file, line, func, olderr, errmsg, ap);
	va_end(ap);

	errno = olderr;
}

void _err_exit(const char *file, int line, const char *func,
	       const char *errmsg, ...)
{
	int olderr = errno;

	va_list ap;
	va_start(ap, errmsg);
	_pr_err(file, line, func, olderr, errmsg, ap);
	va_end(ap);

	exit(olderr);
}

void _err_exit_n(const char *file, int line, const char *func,
	       int errnum, const char *errmsg, ...)
{
	va_list ap;
	va_start(ap, errmsg);
	_pr_err(file, line, func, errnum, errmsg, ap);
	va_end(ap);

	exit(errnum);
}
