#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/errno.h>
#include <linux/user.h>
#include <linux/security.h>
#include <linux/unistd.h>
#include <linux/notifier.h>
#include <linux/proc_fs.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/utsname.h>
#include <linux/errno.h>

#include <asm/unistd.h>
#include <linux/mm.h>         //page_address();
#include <linux/mm_types.h>   //struct page;
#include <asm/pgtable.h>

#include <linux/sched.h>

#define __DEBUG__

#ifdef __DEBUG__
    #define DEBUGLOG(a) printk a
#else
    #define DEBUGLOG(a) ""
#endif

static void __exit exit_prot(void){

	DEBUGLOG(("[* 42Tdbg] - Test Module exit\n\n"));

}

static int __init init_prot(void){

//	struct desc_ptr *dpr = NULL;
	struct page *k_page   = NULL;
	unsigned long vk_page = NULL;
	pte_t *kpte;
	unsigned char* str= "test_module";
	unsigned long xer;

	DEBUGLOG(("[*** 42Tdbg] - Test Module Enter %s\n", str));

	__asm__ volatile ("mov %%cr3, %0\n":"=r"(xer));
	DEBUGLOG (("[*** 42Tdbg] - cr3 = %ul", xer));

//	asm volatile ( "sgdt %0" : "=m"(*dtr));

/**** С памятью руками ***/

//	k_page = virt_to_page(str); // дресс дескриптора страницы

//	DEBUGLOG(("[* 42Tdbg] - k_page = %X |\ flags = %X | \ mapping = %X | \ virtual = %X \\n", k_page, k_page->flags, k_page->mapping, NULL));

//	vk_page = page_address(k_page); // линейный адресс ассоциированный со страничным кадром

//	DEBUGLOG (("[* 42Tdbg] - vk_page = %X", k_page->virtual));

/*	unsigned long *my_decr = my_gdt + (vk_page >> 22);
	DEBUGLOG(("[prot_0] - Offset in GDT = %X \n", my_decr ));

	pud_t *pud = pud_offset(my_decr, vk_page); //Запись верхнего каталога страниц
	DEBUGLOG (("[prot_0] -  pud = %X | %X\n", pud, *pud));
	
	pmd_t *pmd = pmd_offset(pud, vk_page); //Запись средего каталога страниц*/
/*************************/

/**** Старт снятия защиты записи ****/
/*	kpte = pte_offset_kernel(pmd, vk_page); //Запись ТАБЛИЦЫ страниц
	DEBUGLOG (("[prot_0] - kpte = %X | %X \n", kpte, *kpte ));

	DEBUGLOG (("[prot_0] - !!!! = %X | %X\n", pte_to_pgoff(*kpte), pte_page(*kpte)));

	DEBUGLOG(("[prot_0] - writeflag_1 = %i | writeflag_2 = %i | readflag = %i | oungflag = %i\n", (kpte)->pte_low & _PAGE_RW, (*kpte).pte_low & _PAGE_RW,0,0));
	
	*kpte = __pte(pte_val(*kpte) | _PAGE_RW);
	
	DEBUGLOG(("[prot_0] - writeflag_1 = %i | writeflag_2 = %i | readflag = %i | oungflag = %i\n", (kpte)->pte_low & _PAGE_RW, (*kpte).pte_low & _PAGE_RW,0,0));

	unsigned long *open_addr = (sys_table_global + __NR_open);
	DEBUGLOG (("[prot_0] - open_addr = %X  \n", *open_addr ));

	orig_open = (void*)*open_addr;
	DEBUGLOG (("[prot_0] - o_sys_open = %X  \n", orig_open ));
	
	*open_addr = hook_open;
	
	unsigned long *open_readdir = (sys_table_global + __NR_readdir);
	DEBUGLOG(("[maklar] - open_readdir = %x\n", *open_readdir));
	
	orig_readdir = (void*)*open_readdir;
	
	*open_readdir = hook_readdir;

//	DEBUGLOG (("[prot_0] - Open dva = %X  \n", *(sys_table_global + __NR_open) ));*/

/*** Конец снятия защиты записи ***/

	return 0;
}

module_init(init_prot);
module_exit(exit_prot);


/* taint-safe */
MODULE_LICENSE("GPL");

 
