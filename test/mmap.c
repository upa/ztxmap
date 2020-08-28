#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <ztxmap.h>

int main(int argc, char **argv)
{
	struct ztxmap_reg reg;
	int fd, ret;
	void *mem;

	if (argc < 3) {
		printf("usage: %s [devname] [size]\n", argv[0]);
		return -1;
	}
	strncpy(reg.name, argv[1], IFNAMSIZ);
	reg.size = atoi(argv[2]);


	printf("open /dev/ztxmap\n");
	fd = open("/dev/ztxmap", O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	printf("ioctl ZTXMAP_REG with %s %lu-byte\n", argv[1], reg.size);
	ret = ioctl(fd, ZTXMAP_REG, &reg);
	if (ret) {
		perror("ioctl ZTXMAP_REG");
		return -1;
	}

	printf("try mmap()\n");
	mem = mmap(0, reg.size, PROT_READ | PROT_WRITE,
		   MAP_LOCKED | MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED) {
		printf("mmap() faield\n");
		perror("mmap()");
		return -1;
	}

	printf("ioctl ZTXMAP_UNREG\n");
	ret = ioctl(fd, ZTXMAP_UNREG, 0);
	if (ret) {
		perror("ioctl ZTXMAP_UNREG");
		return -1;
	}

	return 0;
}
