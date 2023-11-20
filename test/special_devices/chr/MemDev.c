#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

int dev1_registers[5];

struct cdev *cdev;
dev_t devno;

/*
struct file *filp -- 与字符设备文件关联的file结构，由内核创建
char __user *buf -- 从设备文件读取到的数据，需要保存到的位置
                    由系统调用提供该参数
                    来源于用户空间的指针，这类指针都不能被内核代码直接引用，必须使用专门的函数
                        copy_from_user
                        copy_to_user
size_t size -- 请求传输的数据量
loff_t *ppos -- 文件的读写位置，由内核从file结构中取出后，传递进来
*/

/*文件打开函数 响应系统open调用*/
int mem_open(struct inode *inode, struct file *filp)
{
    printk("mem_open\n");
    /*获取次设备号*/
    int num = MINOR(inode->i_rdev);

    if (num == 0)
        filp->private_data = dev1_registers;
    else
        return -ENODEV; // 无效的次设备号

    return 0;
}

/*文件释放函数 响应系统close调用*/
int mem_release(struct inode *inode, struct file *filp)
{
    printk("mem_release\n");
    return 0;
}

/*读函数  响应系统read调用*/
static ssize_t mem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    printk("mem_read\n");
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    int *register_addr = filp->private_data; /*获取设备的寄存器基地址*/

    /*判断读位置是否有效*/
    if (p >= 5 * sizeof(int))
        return 0;
    if (count > 5 * sizeof(int) - p)
        count = 5 * sizeof(int) - p;

    /*读数据到用户空间*/
    if (copy_to_user(buf, register_addr + p, count))
    {
        ret = -EFAULT;
    }
    else
    {
        *ppos += count;
        ret = count;
    }

    return ret;
}

/*写函数  响应系统write调用*/
static ssize_t mem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    printk("mem_write\n");
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    int *register_addr = filp->private_data; /*获取设备的寄存器地址*/

    /*分析和获取有效的写长度*/
    if (p >= 5 * sizeof(int))
        return 0;

    if (count > 5 * sizeof(int) - p)
        count = 5 * sizeof(int) - p;

    /*从用户空间写入数据*/
    if (copy_from_user(register_addr + p, buf, count))
        ret = -EFAULT;
    else
    {
        *ppos += count;
        ret = count;
    }

    return ret;
}

/*文件定位函数  响应系统sleek调用*/
static loff_t mem_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t newpos;

    switch (whence)
    {
    case SEEK_SET:
        newpos = offset;
        break;

    case SEEK_CUR:
        newpos = filp->f_pos + offset;
        break;

    case SEEK_END:
        newpos = 5 * sizeof(int) - 1 + offset;
        break;

    default:
        return -EINVAL;
    }

    if ((newpos < 0) || (newpos > 5 * sizeof(int)))
        return -EINVAL;

    filp->f_pos = newpos;

    return newpos;
}

/*文件操作结构体*/
static const struct file_operations mem_fops =
    {
        .open = mem_open,
        .read = mem_read,
        .write = mem_write,
        .llseek = mem_llseek,
        .release = mem_release,
};

/*设备驱动模块加载函数*/
static int memdev_init(void)
{
    /* 申请设备号 */
    alloc_chrdev_region(&devno, 0, 1, "memdev");

    /* 分配字符设备 */
    cdev = cdev_alloc();

    /* 设置字符设备 */
    cdev_init(cdev, &mem_fops);

    /* 注册字符设备 */
    cdev_add(cdev, devno, 1);

    return 0;
}

/*模块卸载函数*/
static void memdev_exit(void)
{
    cdev_del(cdev); /*注销设备*/
    kfree(cdev);
    unregister_chrdev_region(devno, 1); /*释放设备号*/
}

MODULE_LICENSE("GPL");

module_init(memdev_init);
module_exit(memdev_exit);