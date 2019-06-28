#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include "common.h"

void _err(const char *file, int line, const char *func,
	  const char *errmsg, ...);
void _err_exit(const char *file, int line, const char *func,
	       const char *errmsg, ...);
void _err_exit_n(const char *file, int line, const char *func,
		 int errnum, const char *errmsg, ...);

#define err(fmt, args...)						\
        _err(__FILE__, __LINE__, __func__, fmt, ##args)
#define err_exit(fmt, args...)						\
        _err_exit(__FILE__, __LINE__, __func__, fmt, ##args)
#define err_exit_n(errnum, fmt, args...)				\
        _err_exit_n(__FILE__, __LINE__, __func__, errnum, fmt, ##args)

#endif /* ERROR_H_ */
