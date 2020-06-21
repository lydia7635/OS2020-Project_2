#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512
#define MAX_FILENUM 100
size_t get_filesize(const char* filename);//get the size of the input file


int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int dev_fd, file_fd;// the fd for the device and the fd for the input file
	int file_num;
	size_t ret, file_size, total_size = 0, offset = 0, tmp;
	char file_name[MAX_FILENUM][50], method[20];
	char *kernel_address = NULL, *file_address = NULL;
	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed

	file_num = atoi(argv[1]);
	printf("file_num = %d\n", file_num);

	for(int i = 0; i < file_num; ++i) {
		strncpy(file_name[i], argv[i + 2], 50);
		printf("filename = %s\n", file_name[i]);
	}
	strncpy(method, argv[file_num + 2], 20);
	printf("method = %s\n", method);

	if( (dev_fd = open("/dev/master_device", O_RDWR)) < 0)
	{
		perror("failed to open /dev/master_device\n");
		return 1;
	}

	gettimeofday(&start ,NULL);
	for(int i = 0; i < file_num; ++i) {
		if( (file_fd = open (file_name[i], O_RDWR)) < 0 )
		{
			perror("failed to open input file\n");
			return 1;
		}

		if( (file_size = get_filesize(file_name[i])) < 0)
		{
			perror("failed to get filesize\n");
			return 1;
		}

		total_size += file_size;

		if(ioctl(dev_fd, 0x12345677) == -1) //0x12345677 : create socket and accept the connection from the slave
		{
			perror("ioclt server create socket error\n");
			return 1;
		}

		switch(method[0])
		{
			case 'f': //fcntl : read()/write()
				do
				{
					ret = read(file_fd, buf, sizeof(buf)); // read from the input file
					write(dev_fd, buf, ret);//write to the the device
				}while(ret > 0);
				break;
		}

		if(ioctl(dev_fd, 0x12345679) == -1) // end sending data, close the connection
		{
			perror("ioclt server exits error\n");
			return 1;
		}

		close(file_fd);
	}
	
	gettimeofday(&end, NULL);
	trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
	printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time, total_size);

	close(dev_fd);

	return 0;
}

size_t get_filesize(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}