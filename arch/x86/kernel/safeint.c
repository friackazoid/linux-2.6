//#include <linux/asm/safeint.h>
#include <linux/types.h>

void smutex_lock (struct mutext *lock) 
{
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $338, %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "memory");
}

