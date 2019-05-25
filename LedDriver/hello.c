#include <linux/module.h> 

static unsigned int max = 5;
module_param(max, uint, 0);
MODULE_PARM_DESC(max,  "Maximum bytes per IO");

static int __init hello_init(void){
	printk("max=%d\n", max);
	return 0;

}

static void __exit hello_exit(void){
	prink("\n");

}

module_init(hello_init);
module_exit(hello_exit);