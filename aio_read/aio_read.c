#include <sys/types.h>
#include <aio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int SIZE_TO_READ = 100;

int main()
{
	int file = open("test_aio.txt", O_RDONLY, 0);

	if(file == -1)
	{
		printf("Unable to open file!\n");
		return 1;
	}
	// Create buffer
	char *buffer = (char *)calloc(SIZE_TO_READ,1);

	//Create the control block structure
	struct aiocb cb;
	
	// Chua thong tin lan doc
	memset(&cb, 0, sizeof(struct aiocb));
	cb.aio_nbytes = SIZE_TO_READ;
	cb.aio_fildes = file; //Doc tu file nao
	cb.aio_offset = 0; //Doc tu dau`
	cb.aio_buf = buffer; //Doc xong thi luu vao buffer o? dau
	
	// Read!
	if (aio_read(&cb) == -1)
	{
		printf("Unable to create request!\n");
		close(file);
	}
	printf("Request enqueued!\n");

	// Wait until the request has finished
	while(aio_error(&cb) == EINPROGRESS)
	{
		printf("Working...\n");
	}

	// Success?
	int numBytes = aio_return(&cb);

	if(numBytes != -1)
		printf("Success!\n");

	return 0;
}
