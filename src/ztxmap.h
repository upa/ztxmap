#ifndef _ZTXMAP_H_
#define _ZTXMAP_H_

#include <linux/if.h>	/* IFNAMSIZ */

#ifndef __KERNEL__
#include <stdint.h>	/* uintXX_t */
#include <sys/user.h>	/* PAGE_SIZAE */
#endif

struct ztxmap_reg {
	char	name[IFNAMSIZ];	/* underlay device name */
	size_t	size;		/* size of requesting/allocated mem reg size */
};

/* The following are copied from skbuff.h */
#if (65536/PAGE_SIZE + 1) < 16
#define MAX_SKB_FRAGS 16UL
#else
#define MAX_SKB_FRAGS (65536/PAGE_SIZE + 1)
#endif

struct ztxmap_vec {
	uint32_t offset;	/* 32bit means max 4GB mem region */
	uint32_t length;
};

struct ztxmap_tx {
	uint8_t	cnt;			/* # of ztxmap_vec */
	struct ztxmap_vec vec[0];	/* upto MAX_SKB_FRAGS vecs */
};

#define ZTXMAP_REG	_IOW('i', 1, struct ztxmap_reg)
#define ZTXMAP_UNREG	_IO('i', 2)
#define ZTXMAP_TX	_IOW('i', 3, struct ztxmap_tx)



#endif
