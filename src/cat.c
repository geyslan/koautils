#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "error.h"

#define BUFSIZE 8192
char buf[BUFSIZE];

static inline void closefd(int fd)
{
	if (close(fd) < 0)
		err_exit("closing fd %d", fd);
}

void cat(int in_fd)
{
	ssize_t nread;
	ssize_t nwritten;

	while((nread = read(in_fd, buf, BUFSIZE)) > 0) {
		if ((nwritten = write(STDOUT_FILENO, buf, nread)) < 0)
			err_exit("writing to stdout");
		if (nwritten == nread)
			continue;
		err_exit_n(EXIT_FAILURE,
			   "written (%d) fewer bytes than read (%d)",
			   nwritten, nread);
	}
	if (nread < 0)
		err_exit("reading from fd %d", in_fd);
}

int is_same_rfile(int in_fd, char *fname)
{
	int out_fd = STDOUT_FILENO;
	struct stat sb;

	dev_t out_dev, in_dev;
	ino_t out_ino, in_ino;
	int out_isreg, in_isreg;
	int flags;

	if (fstat(in_fd, &sb) < 0)
		err_exit("fstat fd %d", in_fd);
	in_dev = sb.st_dev;
	in_ino = sb.st_ino;
	in_isreg = S_ISREG(sb.st_mode);

	if (fstat(out_fd, &sb) < 0)
		err_exit("fstat fd %d", out_fd);
	out_dev = sb.st_dev;
	out_ino = sb.st_ino;
	out_isreg = S_ISREG(sb.st_mode);

	if (!(out_isreg && in_isreg &&
	    out_dev == in_dev &&
	    out_ino == in_ino))
		return 0;

	if (!fname)
		fname = "<redirected>";
	err("ignoring input file (%s) "
	    "since it's the same as the output", fname);
	if ((flags = fcntl(out_fd, F_GETFL)) < 0)
		err_exit("fcntl fd %d", out_fd);
	if (!(flags & O_APPEND))
		err("output file (%s) wasn't open in "
		    "append mode then probably truncated", fname);
	return 1;
}

int main(int argc, char *argv[])
{
	char **file = argv;
	int in_fd = STDIN_FILENO;
	int ret = EXIT_SUCCESS;

	app_set_name(argv[0]);

	if (argc == 1) {
		if (is_same_rfile(in_fd, NULL))
			err_exit("input = output");
		cat(in_fd);
		return ret;
	}

	closefd(in_fd);
	while (*(++file)) {
		if ((in_fd = open(*file, O_RDONLY)) < 0) {
			err("opening file (%s)", *file);
			ret = EXIT_FAILURE;
			continue;
		}
		if (is_same_rfile(in_fd, *file))
			ret = EXIT_FAILURE;
		else
			cat(in_fd);
		closefd(in_fd);
	}
	return ret;
}
