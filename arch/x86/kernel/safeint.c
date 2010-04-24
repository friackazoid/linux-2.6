//#include <linux/asm/safeint.h>
#include <linux/types.h>
#include <asm/unistd.h>

#include <linux/syscalls.h>
#define __STR(X) #X			
#define STR(X) __STR(X)

#define MC_FUNC0(name) MC_FUNCx(0, name)
#define MC_FUNC1(name, ...) MC_FUNCx(1, name, __VA_ARGS__)
#define MC_FUNC2(name, ...) MC_FUNCx(2, name, __VA_ARGS__)
#define MC_FUNC3(name, ...) MC_FUNCx(3, name, __VA_ARGS__)
#define MC_FUNC4(name, ...) MC_FUNCx(4, name, __VA_ARGS__)
#define MC_FUNC5(name, ...) MC_FUNCx(5, name, __VA_ARGS__)
#define MC_FUNC6(name, ...) MC_FUNCx(6, name, __VA_ARGS__)

#define MC_FUNCx(x, name, ...)					\
	void s##name (__SC_DECL##x(__VA_ARGS__))			\
	{							\
		__asm__ __volatile__ (				\
			"\tmovl %0, %%ebx\n"			\
			"\tmovl $"STR(__SR_mod_##name)", %%eax\n"	\
			"\tint $0x80\n"				\
		::"r"(lock) : "eax", "ebx","memory");		\
	}

/*void smutex_lock (struct mutext *lock) 
{

//#define __STR(X) #X
//#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_lock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

//#undef STR
//#undef __STR

}*/

MC_FUNC1(mutex_lock, struct mutext, *lock)
/*
void smutex_lock (struct mutext *lock) { __asm__ __volatile__ ( "\tmovl %0, %%ebx\n" "\tmovl $""_SR_mutex_lock"", %%eax\n" "\tint $0x80\n" ::"r"(lock) : "eax", "ebx","memory"); }
*/

void smutex_unlock (struct mutex *lock)
{

//#define __STR(X) #X
//#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_unlock)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

//#undef STR
//#undef __STR

}

void* skzalloc (size_t size, gfp_t flags) 
{
	unsigned long ret;

//#define __STR(X) #X
//#define STR(X) __STR(X)

	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl $"STR(__SR_mod_kzalloc)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0"
	:"=r" (ret):"r"(size), "r"(flags) :"ebx","ecx", "eax");

//#undef STR
//#undef __STR

	return ret;
}

#undef STR
#undef __STR