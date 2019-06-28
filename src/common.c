#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include "common.h"
#include "error.h"

static char *app_path;
static char *app_name;

static void app_free_path(void);

void app_set_name(char *path)
{
	int path_len;

	if (app_path || !path)
		return;
	path_len = strlen(path) + 1;

	app_path = malloc(path_len * sizeof(*path));
	if (!app_path)
		err_exit("allocating");
	strncpy(app_path, path, path_len);
	app_name = basename(app_path);

	if (atexit(app_free_path))
		err_exit("cannot set exit function");
}

const char *app_get_name(void)
{
	return app_name;
}

static void app_free_path(void)
{
	free(app_path);
}
