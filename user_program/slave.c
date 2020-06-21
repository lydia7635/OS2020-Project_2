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
int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int dev_fd, file_fd;// the fd for the device and the fd for the input file
	int file_num;
	size_t ret, file_size = 0, data_size = -1;
	char file_name[MAX_FILENUM][50], method[20];
	char ip[20];
	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed
	char *kernel_address, *file_address;

	file_num = atoi(argv[1]);
	for(int i = 0; i < file_num; ++i) {
		strncpy(file_name[i], argv[i + 2], 50);
		printf("filename = %s\n", file_name[i]);
	}
	strncpy(method, argv[file_num + 2], 20);
	strncpy(ip, argv[file_num + 3], 20);

	if( (dev_fd = open("/dev/slave_device", O_RDWR)) < 0)//should be O_RDWR for PROT_WRITE when mmap()
	{
		perror("failed to open /dev/slave_device\n");
		return 1;
	}

	gettimeofday(&start ,NULL);
	for(int i = 0; i < file_num; ++i) {
		if( (file_fd = open (file_name[i], O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0)
		{
			perror("failed to open input file\n");
			return 1;
		}

		if(ioctl(dev_fd, 0x12345677, ip) == -1)	//0x12345677 : connect to master in the device
		{
			perror("ioclt create slave socket error\n");
			return 1;
		}

	    write(1, "ioctl success\n", 14);

		switch(method[0])
		{
			case 'f'://fcntl : read()/write()
				do
				{
					ret = read(dev_fd, buf, sizeof(buf)); // read from the the device
					write(file_fd, buf, ret); //write to the input file
					file_size += ret;
				}while(ret > 0);
				break;
		}

		if(ioctl(dev_fd, 0x12345679) == -1)// end receiving data, close the connection
		{
			perror("ioclt client exits error\n");
			return 1;
		}
		close(file_fd);
	}
	
	gettimeofday(&end, NULL);
	trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
	printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time, file_size);

	close(dev_fd);

	return 0;
}


