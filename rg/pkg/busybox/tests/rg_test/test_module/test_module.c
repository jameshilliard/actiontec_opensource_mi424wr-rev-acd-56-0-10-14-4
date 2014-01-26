/* This is a test module for the BB insmod manual testing */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>

int g_data = 9;
int g_bss;
const char *p_rdonly = "a";


int f_local(int in)
{
    return in+3;
}

int init_module()
{
    const char *p = p_rdonly;

    if(*p != 'a' || p[1])
	goto Exit;
    if(f_local(7) != 10)
	goto Exit;
    if (g_data != 9)
	goto Exit;
    if (g_bss != 0)
	goto Exit;
    g_bss = 18; 
    if (g_bss != 18)
	goto Exit;
    
    printk("test_module.o successfully loaded\n");
    return 0;
Exit:
    printk("test_module.o up with problems\n");
    return 0;
}

int cleanup_module()
{
    printk("test_module.o successfully unloaded\n");
    return 0;
}
