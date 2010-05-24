//#include <linux/asm/safeint.h>
#include <linux/types.h>
#include <linux/kmod.h>
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
		"\tmovl %%eax, %0\n"
	:"=r" (ret):"r"(size), "r"(flags) :"ebx","ecx", "eax");

//#undef STR
//#undef __STR

	return ret;
}

int skern_path ( const char *name, unsigned int flags, struct path *path)
{
	unsigned long ret;

	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl %3, %%edx\n"
		"\tmovl $"STR(__SR_mod_kern_path)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0\n"
	:"=r" (ret):"r"(name), "g"(flags), "r"(path) :"ebx","ecx", "edx","eax");

	return ret;
}

int scall_usermodehelper(char *path, char **argv, char **envp, enum umh_wait wait) {
	unsigned long ret;

	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl %3, %%edx\n"
		"\tmovl %4, %%esi\n"
		"\tmovl $"STR(__SR_mod_call_usermodehelper)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0\n"
	:"=r" (ret): "g"(path), "g"(argv), "g"(envp), "g"(wait) :"ebx","ecx", "edx", "esi", "eax");

	return ret;

}

void skfree (const void *objp)
{
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_kfree)", %%eax\n"
		"\tint $0x80\n"
	::"r"(objp) : "eax", "ebx","memory");

}

void smutex_init (struct mutex* lock)
{
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_mutex_init)", %%eax\n"
		"\tint $0x80\n"
	::"r"(lock) : "eax", "ebx","memory");

}

struct dentry* ssecurityfs_create_dir (const char* name, struct dentry *parent)
{
	unsigned long ret;

	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl $"STR(__SR_mod_securityfs_create_dir)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0\n"
	:"=r" (ret): "r"(name), "r"(parent) :"ebx","ecx","eax");

	return ret;

}

struct dentry* ssecurityfs_create_file (const char* name, mode_t mode, struct dentry *parent, void* data, const struct file_operations* fops)
{
	unsigned long ret;

	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl %2, %%ecx\n"
		"\tmovl %3, %%edx\n"
		"\tmovl %4, %%esi\n"
		"\tmovl %5, %%edi\n"
		"\tmovl $"STR(__SR_mod_securityfs_create_file)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0\n"
	:"=r" (ret): "g"(name), "g"(mode), "g"(parent), "g"(data), "g"(fops) :"ebx","ecx", "edx", "esi", "eax");

	return ret;

}
#undef STR
#undef __STR
