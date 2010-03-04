#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/segment.h>


static int __init hello_init(void)
{
	unsigned long cs_s;
	cs_s = 0;

	printk("[Start test ds]\n");
/*		__asm__ __volatile__( "movl %%cs, %0 \n\t"
					: "=r"(cs_s)
					);
	printk("[*] CSis = %X, ( %X ) \n", cs_s, __KERNEL_CS);

	printk("[*] Start set cs: \n");
		__asm__ __volatile__( "movl %0, %%eax  \n\t"
				      "mov %%eax, %%cs \n\t"
					:: "r" (__USER_CS)
					:"%eax"
					);
	printk("[*] Cs is set\n");
		__asm__ __volatile__( "movl %%cs, %0 \n\t"
					: "=r"(cs_s)
					);
	printk("[*] CSis = %X, ( %X ) \n", cs_s, __USER_CS);
*/
	__asm__ __volatile__( "mov %%cr3, %0 \n\t"
					: "=r"(cs_s)
					);
	printk("[*] CSis = %X, ( %X ) \n", cs_s, __KERNEL_CS);


/*
		__asm__ volatile( "mov %0, %%ax \n\t"
				:
				: "r" (cs)
				: "ax"
			);
*/

}
module_init(hello_init);

static void __exit hello_exit(void)
{


}
module_exit(hello_exit);


MODULE_LICENSE("GPL");

