#include <asm/uaccess.h>
#include <linux/init.h>  
#include <linux/init_task.h>
#include <linux/module.h> 
#include <linux/proc_fs.h>  
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <linux/version.h> 
#include <linux/vmalloc.h>

MODULE_AUTHOR("Kalashkov Pavel");
MODULE_LICENSE("GPL");

#define _BUF_SIZE PAGE_SIZE


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static char *_cookie_pot; 

static unsigned read_inx;
static unsigned write_inx;

static struct proc_dir_entry *proc_entry, *proc_dir, *sym_link;
char tmp[256];

static void *fortune_seq_start(struct seq_file *s, loff_t *pos) 
{
  	printk(KERN_INFO "== call seq_start\n");
  	static unsigned long counter = 0;

	if (*pos == 0) 
	{
		return &counter;
	} 
	else 
	{
		*pos = 0;
		return NULL;
	}
}

static void *fortune_seq_next(struct seq_file *s, void *v, loff_t *pos) 
{
	printk(KERN_INFO "== call seq_next\n");
	(*pos)++;
	*(int*)v += 2;//переходим к следующему чётному

    // return v;
	return NULL;
}

static void fortune_seq_stop(struct seq_file *s, void *v) 
{
  	printk(KERN_INFO "== call seq_stop\n");
	if (v)
        printk(KERN_INFO "v is %pX.\n", v);
     else
        printk(KERN_INFO "v is null.\n");
}

static int fortune_seq_show(struct seq_file *s, void *v) 
{
	printk(KERN_INFO "== call seq_show\n");
	int len = 0;

	if (read_inx >= write_inx) 
	{
		read_inx = 0;
	}

	len = sprintf(tmp, "%s\n", &_cookie_pot[read_inx]);
	seq_printf(s, "%s", tmp);

	read_inx += len;
	return 0;
}

static struct seq_operations fortune_seq_ops = 
{
	.start = fortune_seq_start,
	.next = fortune_seq_next,
	.stop = fortune_seq_stop,
	.show = fortune_seq_show
};

static int fortune_open(struct inode *inode, struct file *file) 
{
	printk(KERN_INFO "== call fortune open\n");
	return seq_open(file, &fortune_seq_ops);
};


ssize_t fortune_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp) 
{
	int space_left = (_BUF_SIZE - write_inx) + 1;

	printk(KERN_INFO "== call fortune write");
	if (space_left < count) 
	{
		printk(KERN_INFO "== ERROR: buffer is full. Terminating...");
		return -ENOSPC;
	}
	if (copy_from_user(&_cookie_pot[write_inx], buf, count)) 
	{
		return -EFAULT;
	}

	write_inx += count;
	_cookie_pot[write_inx - 1] = '\0';

	printk(KERN_INFO "== writing successful");
	return count;
}

#ifdef HAVE_PROC_OPS
static struct proc_ops fortune_file_ops = 
{
	.proc_open = fortune_open,
	.proc_read = seq_read,
	.proc_write = fortune_write,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release
};
#else
static struct file_operations fortune_file_ops = 
{
	.owner = THIS_MODULE,
	.open = fortune_open,
	.read = seq_read,
	.write = fortune_write,
	.lseek = seq_lseek,
	.release = seq_release
};
#endif


static int __init fortune_init(void) 
{
	_cookie_pot = (char *)vmalloc(_BUF_SIZE);
	if (!_cookie_pot) 
	{
		printk(KERN_INFO "== ERROR: cant vmalloc!\n");
		return -ENOMEM;
	}
	memset(_cookie_pot, 0, _BUF_SIZE);

	proc_entry = proc_create("seq", S_IRUGO | S_IWUGO, NULL, &fortune_file_ops);
	if (!proc_entry) 
	{
		vfree(_cookie_pot);
		printk(KERN_INFO "== cant proc_create\n");
		return -ENOMEM;
	}

	proc_dir = proc_mkdir("fortune_dir", NULL);
	sym_link = proc_symlink("fortune_symlink", NULL, "/proc/fortune_dir");

	if ((proc_dir == NULL) || (sym_link == NULL)) 
	{
		remove_proc_entry("seq", NULL);
		remove_proc_entry("fortune_symlink", NULL);
		remove_proc_entry("fortune_dir", NULL);
		vfree(_cookie_pot);
		printk(KERN_INFO "== cant proc_create\n");
		return -ENOMEM;
	}

	read_inx = 0;
	write_inx = 0;

	printk(KERN_INFO "== module loaded.");
	return 0;
}

static void __exit fortune_exit(void) 
{
	remove_proc_entry("seq", NULL);
	remove_proc_entry("fortune_symlink", NULL);
	remove_proc_entry("fortune_dir", NULL);

	if (_cookie_pot) 
	{
		vfree(_cookie_pot);
	}

	printk(KERN_INFO "== module unloaded.\n");
}

module_init(fortune_init);
module_exit(fortune_exit);