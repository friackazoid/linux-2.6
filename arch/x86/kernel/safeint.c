//#include <linux/asm/safeint.h>
#include <linux/types.h>

void smutex_lock (struct mutext *lock) 
{
	unsigned char i_stack[1024];
	unsigned char *i_thread_info = 0xc04dbfe0;

	unsigned int i;

//	__asm__ __volatile__ (
//		"\tmovl %%esp,%0\n"
//	:"=r"(i_thread_info)::"eax");

//	i_thread_info = ((unsigned int)i_thread_info & 0xfffff000)+ 0xfaa;

	for(i=0; (unsigned int)(i_thread_info + i) << 20 ;i++ )
		i_stack[i] = (unsigned char) *(i_thread_info + i);

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $338, %%eax\n"
		"\tint $0x80\n"
<<<<<<< HEAD
	::"r"(lock) : "eax");
=======
	::"r"(lock) : "eax", "memory");

	for(; i > 0 ;i-- )
		*(i_thread_info + i) = i_stack[i];


>>>>>>> c9ac7db5edd6bb6df415f8c9a402f1b7d7cde5d7
}

void* skzalloc (size_t size, gfp_t flags) 
{
	unsigned long eax;

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl %1, %%ecx\n"
		"\tmovl $339, %%eax\n"
		"\tint $0x80\n"
	::"r"(size), "r"(flags) : "eax");

	return eax;
}
