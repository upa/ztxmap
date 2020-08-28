#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include <ztxmap.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define ZTXMAP_VERSION	"0.0.0"



/* structure describing ztxmap context. It is created per file
 * descriptor that opens /dev/ztxmap.
 */
struct ztxmap_ctx {
	struct file		*filp;	/* parent file descriptor */
	struct net_device	*dev;	/* underlay netdev */

	void 	*mem;	/* mmap()ed region */
	size_t	size;	/* size of mmap()ed region */
};


static int ztxmap_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int ztxmap_release(struct inode *inode, struct file *filp)
{
	/* TODO: release filp->private_data ztxmap_ctx */
	return 0;
}

static long ztxmap_ioctl(struct file *filp,
			 unsigned int cmd, unsigned long data)
{
	int ret = 0;

	return ret;
}
	

static const struct file_operations ztxmap_fops = {
	.owner		= THIS_MODULE,
	.open		= ztxmap_open,
	.release	= ztxmap_release,
	.unlocked_ioctl	= ztxmap_ioctl,
};

static struct miscdevice ztxmap_mdev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "ztxmap",
	.fops	= &ztxmap_fops,
};

static int __init ztxmap_init(void)
{
	int ret = 0;

	ret = misc_register(&ztxmap_mdev);
	if (ret) {
		pr_err("failed to register miscdevice for ztxmap\n");
		goto out;
	}

	pr_info("%s (v%s) is loaded\n", KBUILD_MODNAME, ZTXMAP_VERSION);

out:
	return ret;
}

static void __exit ztxmap_exit(void)
{
	misc_deregister(&ztxmap_mdev);
	pr_info("%s (v%s) is unloaded\n", KBUILD_MODNAME, ZTXMAP_VERSION);
}


module_init(ztxmap_init);
module_exit(ztxmap_exit);
MODULE_AUTHOR("Ryo Nakamura <upa@haeena.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION(ZTXMAP_VERSION);
