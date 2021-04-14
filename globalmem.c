#include<liunx/init.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/device.h>
#include<linux/err.h>

#define GLOBAL_SIZE 256
#define CLOBALMEM_MAJOR 0
#define GLOBAL_CLEAR 1

static int globalmen_major = CLOBALMEM_MAJOR;  
struct globalmen_dev{
	struct cdev cdev;
	char men[GLOBAL_SIZE];
}

struct globalmen_dev *globalmem_devp = NULL;
dev_t devnum;
//ceshigenggai   
//yuanchengshengji
//目前推送完成，即将进行下载到本地
static ssize_t globalmem_write(struct file *filp, const char __user * buf,
						size_t size, loff_t * ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	struct globalmem_dev *dev = filp->private_data;
	int ret = 0;
	
	if (p >= GLOBAL_SIZE){
		return 0;
	}
	if (count > GLOBAL_SIZE - p){
		count = GLOBAL_SIZE - p;
		printk(KERN_INFO " count = %d\n",count);
	}
	if((ret = copy_from_user(dev->mem +p, buf, count))){
		printk(KERN_INFO "write err %d\n",ret);
		ret = -EFAULT;
	} else {
		*ppos += count;
	        ret = count;	
	}
	printk(KERN_INFO "read %u bytes from %lu, string is %s \n",count, p, dev->mem);
	return ret;
}

static int globalmem_open(struct inode *inode,struct file *filp)
{
	filp->private_data = globalmem_devp;
	return 0;
}

static int globalmem_release(struct inode *inode,struct file *filp)
{
	return 0;
}

static long globalmem_ioctl(struct file *flip, unsigned int cmd,
		unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;
	switch(cmd){
		case GLOBAL_CLEAR:
			memset(dev->mem,0,GLOBAL_SIZE);
			printk(KERN_INFO "mem clear");
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

static ssize_t globalmem_read(struct file *filp, char __user * buf, size_t size, loff_t * ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	struct globalmem_dev *dev = filp ->private_data;
	int ret = 0;
	if (p > GLOBAL_SIZE){
		return 0;
	}

	if (count >= (GLOBAL_SIZE - P)){
		count = CLOBAL_SIZE - P;
	}

	if (copy_to_user(buf,dev->mem + p,count)){
		ret = -EFAULT;
	}else{
		*ppos += count;
		ret = count;

		printk(KERN_INFO "read %u bytes from %lu\n",count,p);
	}
	return ret;
}

static loff_t globalmem_llseek(struct file *filp, loff_toffset, int orig)
{
	return 0;
}

static const struct file_oprerations globalmem_fops = {
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.unlocked_ioctl = globalmem_ioctl,
	.open = globalmem_open,
	.release = globalmem_release,
};

static struct class *global_class;
static struct device *global_class_dev;
static int register_chardev_new(struct globalmem_dev *dev)
{
	globalmem_major = register_chrdev(globalmem_major,"globalmem",&globalmem_fops);
	globalmem_major = globalmem_major;
	if (globalmem_major < 0){
		printk(KERN_INFO "register dev erro \n")
		return globalmem_major
	}

	global_class = class_create(THIS_MODULE,"globalmem");
	if(IS_ERR(global_class)){
		unregister_chrdev(globalmen_major,"globalmen");
		return PTR_ERR(golbal_class);
	}

	printk(KERN_INFO "globalmem_major is %d\n",globalmen_major);
	devnum = MKDEV(globalmem_major,0);
	global_class_dev = device_create(global_clss,NULL,devnum,dev,"globalmem");
}

static int __init globalmem_init(void)
{
	int ret;
	printk(KERN_INFO "globalmen_init\n");
	globalmem_devp = kzalloc(sizeof(struct globalmen_dev),GFP_KERNEL);
	if (!globalmem_devp){
		ret = -ENOMEN;
	}
	register_chardev_new(globalmem_devp);
	return 0;
}
module_init(globalmen_int);



static void __exit globalmem_exit(void)
{
	kfree(globalmem_devp);
#if 0
	unregister_chrdev_region(devnum,1);
	cdev_del(globalmem_devp);
#endif
	device_destroy(global_class,devnum);
	class_destroy(global_class);
	unregsiter_chrdev(globalmen_major,"gloalmen");
	printk(KERN_INFO "globalmem_exit\n");
}
module_exit(globalmen_exit);

MODULE_AUTHOR("CJ <37027592@qq.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple hello World Module");
MODULE_ALIAS("a simplest module");
