//#include <linux/asm/safeint.h>
#include <linux/types.h>
#include <asm/unistd.h>

void smutex_lock (struct mutext *lock) 
{
	unsigned char i_stack[255];
	unsigned char *i_thread_info;

	unsigned int i;

	__asm__ __volatile__ (
		"\tmovl %%esp,%0\n"
	:"=r"(i_thread_info)::"eax");

	i_thread_info = ((unsigned int)i_thread_info & 0xfffff000)+ 0xfec;

	for(i=0; (unsigned int)(i_thread_info + i) << 20 ;i++ )
		i_stack[i] = (unsigned char) *(i_thread_info + i);

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_lock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

#undef STR
#undef __STR

	for(; i > 0 ;i-- )
		*(i_thread_info + i) = i_stack[i];


}

void smutex_unlock (struct mutex *lock)
{
	unsigned char i_stack[255];
	unsigned char *i_thread_info;

	unsigned int i;

	__asm__ __volatile__ (
		"\tmovl %%esp,%0\n"
	:"=r"(i_thread_info)::"eax");

	i_thread_info = ((unsigned int)i_thread_info & 0xfffff000)+ 0xfec;

	for(i=0; (unsigned int)(i_thread_info + i) << 20 ;i++ )
		i_stack[i] = (unsigned char) *(i_thread_info + i);

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_unlock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

#undef STR
#undef __STR

	for(; i > 0 ;i-- )
		*(i_thread_info + i) = i_stack[i];


}

void* skzalloc (size_t size, gfp_t flags) 
{
	unsigned long eax;
	unsigned char i_stack[255];
	unsigned char *i_thread_info;

	unsigned int i;

	__asm__ __volatile__ (
		"\tmovl %%esp,%0\n"
	:"=r"(i_thread_info));

	i_thread_info = ((unsigned int)i_thread_info & 0xfffff000) + 0xfec;

	for(i=0; (unsigned int)(i_thread_info + i) << 20 ;i++ )
		i_stack[i] = (unsigned char) *(i_thread_info + i);

#define __STR(X) #X
#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl %1, %%ecx\n"
		"\tmovl $"STR(__SR_mod_kzalloc)", %%eax\n"
		"\tint $0x80\n"
	::"r"(size), "r"(flags) :"ebx","ecx", "eax");
#undef STR
#undef __STR

	for(; i > 0 ;i-- )
		*(i_thread_info + i) = i_stack[i];

	return eax;
}
