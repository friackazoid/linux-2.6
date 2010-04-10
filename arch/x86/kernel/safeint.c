//#include <linux/asm/safeint.h>
#include <linux/types.h>
#include <asm/unistd.h>

void smutex_lock (struct mutext *lock) 
{

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %%esp,%%esi\n"
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_lock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

#undef STR
#undef __STR

}

void smutex_unlock (struct mutex *lock)
{

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %%esp,%%esi\n"
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_unlock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

#undef STR
#undef __STR

}

void* skzalloc (size_t size, gfp_t flags) 
{
	unsigned long ret;

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %%esp,%%esi\n"
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl $"STR(__SR_mod_kzalloc)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0"
	:"=r" (ret):"r"(size), "r"(flags) :"ebx","ecx", "eax");
#undef STR
#undef __STR

	return ret;
}
