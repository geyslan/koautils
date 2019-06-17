#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 8192
char buf[BUFSIZE];

void cat(int in_fd)
{
	ssize_t nread;
	ssize_t nwritten;

	while((nread = read(in_fd, buf, BUFSIZE)) > 0) {
		if ((nwritten = write(STDOUT_FILENO, buf, nread)) < 0) {
			// treat error
			exit(EXIT_FAILURE);
		}
	}

	if (nread < 0) {
		// treat error
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	char **file = argv;
	int in_fd = STDIN_FILENO;
	struct stat sb;
	dev_t out_dev;
	ino_t out_ino;
	int out_isreg;
	int out_flags;
	int ret = EXIT_SUCCESS;

	if (argc == 1) {
		cat(in_fd);
		return ret;
	}

	if (fstat(STDOUT_FILENO, &sb) < 0) {
		//treat error
		exit(EXIT_FAILURE);
	}
	out_dev = sb.st_dev;
	out_ino = sb.st_ino;
	out_isreg = (sb.st_mode & S_IFMT) == S_IFREG;
	if ((out_flags = fcntl(STDOUT_FILENO, F_GETFL)) < 0) {
		// treat error
		exit(EXIT_FAILURE);
	}

	while (*(++file)) {
		if ((in_fd = open(*file, O_RDONLY)) < 0) {
			// treat error
			fprintf(stderr, "> error opening file: %s\n", *file);
			ret = EXIT_FAILURE;
			continue;
		}
		if (fstat(in_fd, &sb) < 0) {
			//treat error
		}
		if (out_isreg &&
			out_dev == sb.st_dev && out_ino == sb.st_ino) {
			ret = EXIT_FAILURE;
			// treat error
			fprintf(stderr, "> ignoring input file %s"
					" since it is the same output\n", *file);
			if (!(out_flags & O_APPEND))
				fprintf(stderr, "> output file %s isn't open in append mode"
					" then probably overwritten\n", *file);
		} else {
			cat(in_fd);
		}
		if (close(in_fd) < 0) {
			// treat error
			exit(EXIT_FAILURE);
		}
	}
	return ret;
}
