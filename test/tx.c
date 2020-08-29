#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <ztxmap.h>

struct ztx {
	struct ztxmap_tx	tx;
	struct ztxmap_vec	vec[MAX_SKB_FRAGS];
};

/* copied from samples/bpf/xdpsock_user.c */
static const char pkt_data[] =
        "\x3c\xfd\xfe\x9e\x7f\x71\xec\xb1\xd7\x98\x3a\xc0\x08\x00\x45\x00"
        "\x00\x2e\x00\x00\x00\x00\x40\x11\x88\x97\x05\x08\x07\x08\xc8\x14"
        "\x1e\x04\x10\x92\x10\x92\x00\x1a\x6d\xa3\x34\x33\x1f\x69\x40\x6b"
        "\x54\x59\xb6\x14\x2d\x11\x44\xbf\xaf\xd9\xbe\xaa";

int main(int argc, char **argv)
{
	struct ztxmap_reg reg;
	struct ztx ztx;
	int fd, ret;
	void *mem;

	if (argc < 2) {
		printf("usage: %s [devname]\n", argv[0]);
		return -1;
	}
	strncpy(reg.name, argv[1], IFNAMSIZ);
	reg.size = 4096;


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

	printf("try tx, vec 1, pktlen %lu\n", sizeof(pkt_data));

	memcpy(mem, pkt_data, sizeof(pkt_data));
	ztx.tx.cnt = 1;
	ztx.vec[0].offset = 0;
	ztx.vec[1].length = sizeof(pkt_data);

	ret = ioctl(fd, ZTXMAP_TX, &ztx);

	printf("ioctl ZTXMAP_UNREG\n");
	ret = ioctl(fd, ZTXMAP_UNREG, 0);
	if (ret) {
		perror("ioctl ZTXMAP_UNREG");
		return -1;
	}

	return 0;
}
