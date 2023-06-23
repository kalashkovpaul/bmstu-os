#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "ascii_names.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalashkov Pavel");

struct tasklet_struct *s_tasklet;
char my_tasklet_data[] = "key is pressed";

int keyboard_irq = 1;

void my_tasklet_func(unsigned long data) 
{
  printk(KERN_INFO "+ Tasklet: -------------------------\n");
  printk(KERN_INFO "+ Tasklet: tasklet count = %u\n", s_tasklet->count.counter);
  printk(KERN_INFO "+ Tasklet: tasklet state = %lu\n", s_tasklet->state);

  printk(KERN_INFO "+ Tasklet: key code is %d\n", data);

  if (data < 84)
    printk(KERN_INFO "+ Tasklet: the key is %s\n", ascii_names[data]);

  printk(KERN_INFO "+ Tasklet: -------------------------\n");
}

irqreturn_t my_irq_handler(int irq, void *dev) 
{
  printk(KERN_INFO "+ Tasklet: my_irq_handler\n");
  if (irq == keyboard_irq) 
  {
    printk(KERN_INFO "+ Tasklet: called by keyboard_irq\n");
    printk(KERN_INFO "+ Tasklet: tasklet state (before) = %lu\n",
      s_tasklet->state);

    int code = inb(0x60);
    s_tasklet->data = code;

    printk(KERN_INFO "+ Tasklet: scheduling\n");
    tasklet_schedule(s_tasklet);

    printk(KERN_INFO "+ Tasklet: tasklet state (after) = %lu\n",
      s_tasklet->state);

    return IRQ_HANDLED;
  }

  printk(KERN_INFO "+ Tasklet: called not by keyboard_irq\n");
  return IRQ_NONE;
}

static int __init my_tasklet_init(void) 
{
  int res = request_irq(keyboard_irq, my_irq_handler, IRQF_SHARED,
    "test_my_irq_handler", (void *)my_irq_handler);
  if (res) 
  {
    printk(KERN_ERR "+ Tasklet: request_irq() error\n");
    return -1;
  }
  s_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
  if (s_tasklet == NULL) 
  {
    printk(KERN_ERR "+ Tasklet: kmalloc() error\n");
    free_irq(keyboard_irq, my_irq_handler);
    return -1;
  }

  printk(KERN_INFO "+ Tasklet: init\n");
  tasklet_init(s_tasklet, my_tasklet_func, (unsigned long)my_tasklet_data);
  printk(KERN_ERR "+ Tasklet: loaded\n");
  return 0;
}

static void __exit my_tasklet_exit(void) 
{
  printk(KERN_INFO "+ Tasklet: exit\n");

  synchronize_irq(keyboard_irq);
  free_irq(keyboard_irq, my_irq_handler);

  tasklet_kill(s_tasklet);
  kfree(s_tasklet);

  printk(KERN_INFO "+ Tasklet: unloaded\n");
}

module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
