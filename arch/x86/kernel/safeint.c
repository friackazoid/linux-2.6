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
	::"r"(lock) : "eax", "memory");

	for(; i > 0 ;i-- )
		*(i_thread_info + i) = i_stack[i];


}

