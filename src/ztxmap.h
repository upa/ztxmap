#ifndef _ZTXMAP_H_
#define _ZTXMAP_H_

#include <linux/if.h>	/* IFNAMSIZ */

struct ztxmap_reg {
	char	name[IFNAMSIZ];	/* underlay device name */
	size_t	size;		/* size of requesting/allocated mem reg size */
};

#define ZTXMAP_REG	_IOW('i', 1, struct ztxmap_reg)
#define ZTXMAP_UNREG	_IO('i', 2)


#endif
