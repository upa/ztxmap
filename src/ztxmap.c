#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/netdevice.h>

#include <ztxmap.h>

#ifdef pr_fmt
#undef pr_fmt
#endif
#define pr_fmt(fmt) KBUILD_MODNAME ":%s(): " fmt, __func__

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
	struct ztxmap_ctx *ctx;

	ctx = kmalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	memset(ctx, 0, sizeof(*ctx));
	filp->private_data = ctx;
	ctx->filp = filp;
	ctx->dev = NULL;
	ctx->mem = NULL;

	return 0;
}

static void ztxmap_unreg(struct ztxmap_ctx *ctx)
{
	if (ctx->mem) {
		pr_debug("free %lu-byte region\n", ctx->size);
		kfree(ctx->mem);
		ctx->mem = NULL;
	}
	if (ctx->dev) {
		pr_debug("put %s\n", ctx->dev->name);
		dev_put(ctx->dev);
		ctx->dev = NULL;
	}
}

static int ztxmap_release(struct inode *inode, struct file *filp)
{
	struct ztxmap_ctx *ctx;

	ctx = filp->private_data;
	ztxmap_unreg(ctx);
	kfree(ctx);

	return 0;
}


static int ztxmap_ioctl_reg(struct file *filp, unsigned long data)
{
	struct ztxmap_ctx *ctx;
	struct net_device *dev;
	struct ztxmap_reg reg;
	int ret = 0;

	ctx = filp->private_data;
	if (ctx->dev || ctx->mem) {
		pr_warn("already registered\n");
		return -EBUSY;
	}

	if (copy_from_user(&reg, (void *)data, sizeof(reg)) != 0)
		return -EFAULT;

	dev = dev_get_by_name(current->nsproxy->net_ns, reg.name);
	if (!dev) {
		pr_warn("device '%s' not found\n", reg.name);
		return -ENODEV;
	}
	ctx->dev = dev;

	ctx->mem = kmalloc(reg.size, GFP_KERNEL | __GFP_COMP);
	if (!ctx->mem) {
		pr_warn("failed to allocate %lu-byte memory\n", reg.size);
		ret = -ENOMEM;
		goto err_out;
	}
	ctx->size = reg.size;

	pr_debug("%lu-byte for %s allocated\n", ctx->size, ctx->dev->name);

	return 0;

err_out:
	dev_put(dev);
	return ret;
}

static int ztxmap_ioctl_unreg(struct file *filp)
{
	struct ztxmap_ctx *ctx;

	ctx = filp->private_data;
	ztxmap_unreg(ctx);
	/* ctx is kfree()ed by ztxmap_release() */
	return 0;
}

static long ztxmap_ioctl(struct file *filp,
			 unsigned int cmd, unsigned long data)
{
	int ret = 0;

	switch (cmd) {
	case ZTXMAP_REG:
		ret = ztxmap_ioctl_reg(filp, data);
		break;
	case ZTXMAP_UNREG:
		ret = ztxmap_ioctl_unreg(filp);
		break;
	default:
		return -EINVAL;
	}

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
	.mode	= 00666,
};


static int __init ztxmap_init(void)
{
	int ret = 0;

	ret = misc_register(&ztxmap_mdev);
	if (ret) {
		pr_err("failed to register miscdevice for ztxmap\n");
		goto out;
	}

	pr_info("v%s is loaded\n", ZTXMAP_VERSION);

out:
	return ret;
}

static void __exit ztxmap_exit(void)
{
	misc_deregister(&ztxmap_mdev);
	pr_info("v%s is unloaded\n", ZTXMAP_VERSION);
}


module_init(ztxmap_init);
module_exit(ztxmap_exit);

MODULE_AUTHOR("Ryo Nakamura <upa@haeena.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION(ZTXMAP_VERSION);
