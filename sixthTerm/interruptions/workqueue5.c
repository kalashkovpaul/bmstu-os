#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "ascii_names.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kalashkov Pavel");

#define SLEEP_TIME 12

typedef struct {
  struct work_struct work;
  int code;
} my_work_struct_t;

static struct workqueue_struct *wq;

static my_work_struct_t *work1;
static struct work_struct *work2;

int keyboard_irq = 1;

void work1_func(struct work_struct *work) 
{
  my_work_struct_t *my_work = (my_work_struct_t *)work;
  int code = my_work->code;

  printk(KERN_INFO "+ workqueue: work1 begin\n");
  printk(KERN_INFO "+ workqueue: key code is %d\n", code);
  if (code < 84)
    printk(KERN_INFO "+ workqueue: the key is %s\n", ascii_names[code]);
  printk(KERN_INFO "+ workqueue: work1 end\n");
}

void work2_func(struct work_struct *work) 
{
  printk(KERN_INFO "+ workqueue: work2 sleep begin\n");
  msleep(SLEEP_TIME);
  printk(KERN_INFO "+ workqueue: work2 sleep end, slept %lu jiffies\n", msecs_to_jiffies(SLEEP_TIME));
}

irqreturn_t my_irq_handler(int irq, void *dev) 
{
  printk(KERN_INFO "+ workqueue: my_irq_handler\n");
  if (irq == keyboard_irq) 
  {
    printk(KERN_INFO "+ workqueue: called by keyboard_irq\n");

    int code = inb(0x60);
    work1->code = code;

    queue_work(wq, (struct work_struct *)work1);
    queue_work(wq, work2);

    return IRQ_HANDLED;
  }

  printk(KERN_INFO "+ workqueue: called not by keyboard_irq\n");
  return IRQ_NONE;
}

static int __init my_workqueue_init(void) 
{
  int ret = request_irq(keyboard_irq, my_irq_handler, IRQF_SHARED,
    "test_my_irq_handler", (void *)my_irq_handler);
  if (ret) 
  {
    printk(KERN_ERR "+ workqueue: request_irq error\n");
    return -1;
  }

  printk(KERN_INFO "+ workqueue: init\n");
  wq = alloc_workqueue("%s", __WQ_LEGACY | WQ_MEM_RECLAIM, 1, "workqueue");
  if (wq == NULL) 
  {
    printk(KERN_ERR "+ workqueue: create queue error\n");
    free_irq(keyboard_irq, my_irq_handler);
    return -1;
  }

  work1 = kmalloc(sizeof(my_work_struct_t), GFP_KERNEL);
  if (work1 == NULL) 
  {
    printk(KERN_ERR "+ workqueue: work1 alloc error\n");
    destroy_workqueue(wq);
    free_irq(keyboard_irq, my_irq_handler);
    return -1;
  }

  work2 = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
  if (work2 == NULL) 
  {
    printk(KERN_ERR "+ workqueue: work2 alloc error\n");
    destroy_workqueue(wq);
    kfree(work1);
    free_irq(keyboard_irq, my_irq_handler);
    return -1;
  }

  INIT_WORK((struct work_struct *)work1, work1_func);
  INIT_WORK(work2, work2_func);

  printk(KERN_INFO "+ workqueue: loaded\n");
  return 0;
}

static void __exit my_workqueue_exit(void) 
{
  printk(KERN_INFO "+ workqueue: exit\n");

  flush_workqueue(wq);
  printk(KERN_INFO "+ workqueue: flushed\n");
  destroy_workqueue(wq);
  synchronize_irq(keyboard_irq);
  free_irq(keyboard_irq, my_irq_handler);

  kfree(work1);
  kfree(work2);

  printk(KERN_INFO "+ workqueue: unloaded\n");
}

module_init(my_workqueue_init);
module_exit(my_workqueue_exit);
