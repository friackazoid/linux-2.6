//#include <linux/asm/safeint.h>
#include <linux/types.h>

void smutex_lock (struct mutext *lock) 
{
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $338, %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax");
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
