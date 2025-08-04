#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void main()
{
	int fd;
	int fd1;
	fd = open("mytext1", O_CREAT | O_WRONLY);
	fd1 = open("mytext2", O_CREAT | O_WRONLY);
	printf("fd1 = %d, fd2 = %d/n", fd,fd1);
	write(fd, "Hello world", strlen("Hello world"));
	sync();
	close(fd);
}
