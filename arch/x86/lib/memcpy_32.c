#include <linux/string.h>
#include <linux/module.h>
#include <linux/syscalls.h>

#undef memcpy
#undef memset

void *memcpy(void *to, const void *from, size_t n)
{
#ifdef CONFIG_X86_USE_3DNOW
	return __memcpy3d(to, from, n);
#else
	return __memcpy(to, from, n);
#endif
}
EXPORT_SYMBOL(memcpy);

void* modmemcpy(void* to, const void *from, size_t n)

{
#define __STR(X) #X
#define STR(X) __STR(X)
        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %1, %%ebx\n"
		 "\tmovl %2, %%ecx\n"
		 "\tmovl %3, %%edx\n"
	         "\tmovl $"STR(__SR_modmemcpy)", %%eax\n"
		 "\tint $0x80\n"
		 "\tmovl %%eax, %0"
		 :"=m" (ret):"m"(to), "m"(from), "m"(n): "edx", "ecx", "ebx", "eax");
	return ret;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(modmemcpy);

SYSCALL_DEFINE3 (modmemcpy, void*, to, const void*, from, size_t, n)
{
        return modmemcpy(to, from, n);
}

void *memset(void *s, int c, size_t count)
{
	return __memset(s, c, count);
}
EXPORT_SYMBOL(memset);

void *memmove(void *dest, const void *src, size_t n)
{
	int d0, d1, d2;

	if (dest < src) {
		memcpy(dest, src, n);
	} else {
		__asm__ __volatile__(
			"std\n\t"
			"rep\n\t"
			"movsb\n\t"
			"cld"
			: "=&c" (d0), "=&S" (d1), "=&D" (d2)
			:"0" (n),
			 "1" (n-1+src),
			 "2" (n-1+dest)
			:"memory");
	}
	return dest;
}
EXPORT_SYMBOL(memmove);
