#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/path.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalashkov Pavel");

static int __init my_module_init(void) {
    struct task_struct *task = &init_task;
    printk(KERN_INFO "+ START");
    do {
        if (task->pid == 30359) {
            printk(KERN_INFO "+ %s, pid = %d, parent = %s, ppid = %d\n\
            state = %d, flags = %d, prio = %d, policy = %s, root = %s, pwd = %s\n", 
            task->comm, task->pid, task->parent->comm, task->parent->pid,
            task->__state, task->flags, task->prio, task->policy, 
            task->fs->root.dentry->d_name.name, task->fs->pwd.dentry->d_name.name);
            struct files_struct * files = task->files;
            printk(KERN_INFO "+ Total files count: %d", atomic_read(&files->count));
            printk(KERN_INFO "+ Min free fd: %d", files->next_fd);
            printk(KERN_INFO "+ fdtab %llu", files->fdt);
            printk(KERN_INFO "+ max_fds %u", files->fdt->max_fds);
            printk(KERN_INFO "+ fd table in fdtab %llu", files->fdt->fd);
            for (int i = 3; i < files->next_fd; i++) {
                printk(KERN_INFO "+ fd with index %d", i);
                printk(KERN_INFO "+ file %llu", files->fdt->fd[i]);
                printk(KERN_INFO "+ file inode pointer %llu", files->fdt->fd[i]->f_inode);
                printk(KERN_INFO "+ file inode i_no %lu", files->fdt->fd[i]->f_inode->i_ino);
                printk(KERN_INFO "+ file f_pos %llu", files->fdt->fd[i]->f_pos);
            }
        }

    } while ((task = next_task(task)) != &init_task);
    printk(KERN_INFO "+ END");
    return 0;
}

static void __exit my_module_exit(void) {

}

module_init(my_module_init);
module_exit(my_module_exit);










