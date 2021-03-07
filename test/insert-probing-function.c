#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "probing-nop.h"

#define BUFFER_SIZE		1048576

int main(void)
{
	int fd;
	ssize_t len;
	char *buffer;
	unsigned int idt_vector;
	unsigned long long nop_address;
	unsigned long long pf_address;

	if ((buffer = (char *) malloc(BUFFER_SIZE)) == NULL)
		return -1;

	if ((fd = open("/dev/probing-nop", O_RDONLY)) < 0)
	{
		free((void *) buffer);
		return -1;
	}

	printf("Insert the IDT vector number associated to the interrupt handler that you want to inspect: ");
	scanf("%u", &idt_vector);

	if (ioctl(fd, P_IDT_SELECT, idt_vector) < 0)
	{
		close(fd);
		free((void *) buffer);
		return -1;
	}

	if ((len = read(fd, (void *) buffer, BUFFER_SIZE)) < 0)
	{
		close(fd);
		free((void *) buffer);
		return -1;
	}

	printf("%s", buffer);

	printf("Copy/Paste the NOP address (highlighted in blue) at which you want to insert the call to a probing function: ");
	scanf("%llx", &nop_address);

	if (ioctl(fd, P_NOP_SELECT, nop_address) < 0)
	{
		close(fd);
		free((void *) buffer);
		return -1;
	}

	printf("Copy/Paste the address of the probing function (highlighted in blue) to which you want to give control inline with the handling of the interrupt: ");
	scanf("%llx", &pf_address);

	if (ioctl(fd, P_NOP_UPDATE, pf_address) < 0)
	{
		close(fd);
		free((void *) buffer);
		return -1;
	}

	close(fd);
	free((void *) buffer);
	return 0;
}