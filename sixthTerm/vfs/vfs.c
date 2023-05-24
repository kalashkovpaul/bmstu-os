#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalashkov Pavel");

#define FS_MAGIC 0x01010101
#define SLAB_NAME "my_vfs_cache"

static struct kmem_cache *cache = NULL;
static void **cache_mem_area = NULL;

static void my_vfs_put_super(struct super_block *sb)
{
    printk(KERN_INFO "+ superblock destroy called.\n");
}

static struct super_operations const my_vfs_sup_ops = {
    .put_super = my_vfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static int my_vfs_fill_sb(struct super_block *sb, void *data, int silent)
{
    struct dentry *root;
    struct inode *root_inode;

    sb->s_blocksize = PAGE_SIZE;    
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = FS_MAGIC;
    sb->s_op = &my_vfs_sup_ops;

    root_inode = new_inode(sb);
    if (!root_inode)
    {
        printk(KERN_INFO "+ can't make root inode.");
        return -ENOMEM;
    }

    root_inode->i_ino = 1;
    printk(KERN_INFO "+ root inode = %lu", root_inode->i_ino);

    root_inode->i_mode = S_IFDIR | 0755;

    root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = current_time(root_inode);
    root_inode->i_op = &simple_dir_inode_operations;
    root_inode->i_fop = &simple_dir_operations;

    root = d_make_root(root_inode);
    if (!root)
    {
        printk(KERN_ERR "+ can't d_make_root.");
        return -ENOMEM;
    }

    sb->s_root = root;

    return 0;
}

static struct dentry *my_vfs_mount(struct file_system_type *type, int flags, const char *dev, void *data)
{
    struct dentry *const root_dentry = mount_nodev(type, flags, data, my_vfs_fill_sb);

    if (IS_ERR(root_dentry))
        printk(KERN_ERR "+ can't mount_nodev\n");
    else
        printk(KERN_INFO "+ VFS has been mounted.\n");

    return root_dentry;
}

static void my_kill_sb(struct super_block *sb){
    printk(KERN_INFO "+ kill super block.\n");
    kill_anon_super(sb);
}

static struct file_system_type my_vfs_type = {
    .owner = THIS_MODULE,
    .name = "myvfs",
    .mount = my_vfs_mount,
    .kill_sb = my_kill_sb, 
};

static int __init my_vfs_init(void){
    int result_register = register_filesystem(&my_vfs_type);

    if (result_register != 0){
        printk(KERN_ERR "+ can't register_filesystem.");
        return result_register;
    }

    if ((cache_mem_area = kmalloc(sizeof(void*), GFP_KERNEL)) == NULL)
    {
        printk(KERN_ERR "+ failed to allocate memory.\n");
        return -ENOMEM;
    }

    if ((cache = kmem_cache_create(SLAB_NAME, 7, 0, SLAB_HWCACHE_ALIGN, NULL)) == NULL)
    {
        printk(KERN_ERR "+ can't kmem_cache_create.\n");
        kfree(cache_mem_area);
        return -ENOMEM;
    }

    if ((*cache_mem_area = kmem_cache_alloc(cache, GFP_KERNEL))== NULL)
    {
        printk(KERN_ERR "+ can't kmem_cache_create.\n");
        kfree(cache_mem_area);
        kmem_cache_destroy(cache);
        return -ENOMEM;
    }

    printk(KERN_INFO "+ module loaded.");
    return 0;
}

static void __exit my_vfs_exit(void)
{
    kmem_cache_free(cache, *cache_mem_area);
    kfree(cache_mem_area);
    kmem_cache_destroy(cache);

    int result_unregister = unregister_filesystem(&my_vfs_type);
    if (result_unregister != 0){
        printk(KERN_ERR "+ can't unregister file system.");
    }
    else{
        printk(KERN_INFO "+ module is unloaded.");
    }
}

module_init(my_vfs_init);
module_exit(my_vfs_exit);
