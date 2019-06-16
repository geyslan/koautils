#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 8192
char buf[BUFSIZE];

void cat(int fd)
{
	ssize_t nread;
	ssize_t nwritten;

	while((nread = read(fd, buf, BUFSIZE)) > 0) {
		nwritten = write(STDOUT_FILENO, buf, nread);
		if (nwritten == -1) {
			// treat error
			exit(2);
		}
	}

	if (nread == -1) {
		// treat error
		exit(2);
	}
}

int main(int argc, char *argv[])
{
	char **file = argv;

	if (argc == 1) {
		cat(STDIN_FILENO);
		return 0;
	}

	while (*(++file)) {
		int fd = open(*file, O_RDONLY);

		if (fd == -1) {
			// treat error
			fprintf(stderr, "> error opening file: %s\n", *file);
			continue;
		}
		cat(fd);
		if (close(fd) == -1) {
			// treat error
			exit(2);
		}
	}
	return 0;
}
