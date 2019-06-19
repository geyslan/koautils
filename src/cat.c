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

int is_same_rfile(int in_fd, int out_fd, char *fname)
{
	struct stat sb;

	dev_t out_dev, in_dev;
	ino_t out_ino, in_ino;
	int out_isreg, in_isreg;
	int flags;

	if (fstat(in_fd, &sb) < 0) {
		//treat error
		exit(EXIT_FAILURE);
	}
	in_dev = sb.st_dev;
	in_ino = sb.st_ino;
	in_isreg = S_ISREG(sb.st_mode);

	if (fstat(out_fd, &sb) < 0) {
		//treat error
		exit(EXIT_FAILURE);
	}
	out_dev = sb.st_dev;
	out_ino = sb.st_ino;
	out_isreg = S_ISREG(sb.st_mode);

	if ((out_isreg && in_isreg) &&
		out_dev == in_dev && out_ino == in_ino) {
		fprintf(stderr, "> ignoring input file (%s) "
			"since it's the same as the output\n", fname);
		if ((flags = fcntl(out_fd, F_GETFL)) < 0) {
			// treat error
		}
		if (!(flags & O_APPEND))
			fprintf(stderr, "> output file (%s) wasn't open in "
				"append mode then probably truncated\n", fname);
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char **file = argv;
	int in_fd = STDIN_FILENO;
	int ret = EXIT_SUCCESS;

	if (argc == 1) {
		if (is_same_rfile(in_fd, STDOUT_FILENO, "<redirected>")) {
			// treat error
			exit(EXIT_FAILURE);
		}
		cat(in_fd);
		return ret;
	}

	if (close(in_fd) < 0) {
		// treat error
		exit(EXIT_FAILURE);
	}
	while (*(++file)) {
		if ((in_fd = open(*file, O_RDONLY)) < 0) {
			// treat error
			fprintf(stderr, "> error opening file (%s)\n", *file);
			ret = EXIT_FAILURE;
			continue;
		}
		if (is_same_rfile(in_fd, STDOUT_FILENO, *file)) {
			// treat error
			ret = EXIT_FAILURE;
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
