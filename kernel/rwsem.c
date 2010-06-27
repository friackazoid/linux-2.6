/* kernel/rwsem.c: R/W semaphores, public implementation
 *
 * Written by David Howells (dhowells@redhat.com).
 * Derived from asm-i386/semaphore.h
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/rwsem.h>

#include <asm/system.h>
#include <asm/atomic.h>
#include <linux/syscalls.h>

/*
 * lock for reading
 */
void __sched down_read(struct rw_semaphore *sem)
{
	might_sleep();
	rwsem_acquire_read(&sem->dep_map, 0, 0, _RET_IP_);

	LOCK_CONTENDED(sem, __down_read_trylock, __down_read);
}

EXPORT_SYMBOL(down_read);

void __sched moddown_read(struct rw_semaphore *sem)
{
#define __STR(X) #X
#define STR(X) __STR(X)
//        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %0, %%ebx\n"
	         "\tmovl $"STR(__SR_moddown_read)", %%eax\n"
		 "\tint $0x80\n"
		 ::"m"(sem): "ebx", "eax");
	return;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(moddown_read);

SYSCALL_DEFINE1 (moddown_read, struct rw_semaphore*, sem)
{
        down_read(sem);
	return 0;
}


/*
 * trylock for reading -- returns 1 if successful, 0 if contention
 */
int down_read_trylock(struct rw_semaphore *sem)
{
	int ret = __down_read_trylock(sem);

	if (ret == 1)
		rwsem_acquire_read(&sem->dep_map, 0, 1, _RET_IP_);
	return ret;
}

EXPORT_SYMBOL(down_read_trylock);

/*
 * lock for writing
 */
void __sched down_write(struct rw_semaphore *sem)
{
	might_sleep();
	rwsem_acquire(&sem->dep_map, 0, 0, _RET_IP_);

	LOCK_CONTENDED(sem, __down_write_trylock, __down_write);
}

EXPORT_SYMBOL(down_write);

void __sched moddown_write(struct rw_semaphore *sem)
{
#define __STR(X) #X
#define STR(X) __STR(X)
//        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %0, %%ebx\n"
	         "\tmovl $"STR(__SR_moddown_write)", %%eax\n"
		 "\tint $0x80\n"
		 ::"m"(sem): "ebx", "eax");
	return;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(moddown_write);

SYSCALL_DEFINE1 (moddown_write, struct rw_semaphore *, sem)
{
        down_write(sem);
	return 0;
}


/*
 * trylock for writing -- returns 1 if successful, 0 if contention
 */
int down_write_trylock(struct rw_semaphore *sem)
{
	int ret = __down_write_trylock(sem);

	if (ret == 1)
		rwsem_acquire(&sem->dep_map, 0, 1, _RET_IP_);
	return ret;
}

EXPORT_SYMBOL(down_write_trylock);

/*
 * release a read lock
 */
void up_read(struct rw_semaphore *sem)
{
	rwsem_release(&sem->dep_map, 1, _RET_IP_);

	__up_read(sem);
}

EXPORT_SYMBOL(up_read);


void modup_read(struct rw_semaphore *sem)
{
#define __STR(X) #X
#define STR(X) __STR(X)
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_modup_read)", %%eax\n"
		"\tint $0x80\n"
		::"m"(sem) :"ebx", "eax");
#undef STR
#undef __STR
}
SYSCALL_DEFINE1(modup_read, struct rw_semaphore *, sem)
{
	up_read(sem);
	return 0;
}


/*
 * release a write lock
 */
void up_write(struct rw_semaphore *sem)
{
	rwsem_release(&sem->dep_map, 1, _RET_IP_);

	__up_write(sem);
}

EXPORT_SYMBOL(up_write);

void __sched modup_write(struct rw_semaphore *sem)
{
#define __STR(X) #X
#define STR(X) __STR(X)
//        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %0, %%ebx\n"
	         "\tmovl $"STR(__SR_modup_write)", %%eax\n"
		 "\tint $0x80\n"
		 ::"m"(sem): "ebx", "eax");
	return;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(modup_write);

SYSCALL_DEFINE1 (modup_write, struct rw_semaphore *, sem)
{
        up_write(sem);
	return 0;
}

/*
 * downgrade write lock to read lock
 */
void downgrade_write(struct rw_semaphore *sem)
{
	/*
	 * lockdep: a downgraded write will live on as a write
	 * dependency.
	 */
	__downgrade_write(sem);
}

EXPORT_SYMBOL(downgrade_write);

#ifdef CONFIG_DEBUG_LOCK_ALLOC

void down_read_nested(struct rw_semaphore *sem, int subclass)
{
	might_sleep();
	rwsem_acquire_read(&sem->dep_map, subclass, 0, _RET_IP_);

	LOCK_CONTENDED(sem, __down_read_trylock, __down_read);
}

EXPORT_SYMBOL(down_read_nested);

void down_read_non_owner(struct rw_semaphore *sem)
{
	might_sleep();

	__down_read(sem);
}

EXPORT_SYMBOL(down_read_non_owner);

void down_write_nested(struct rw_semaphore *sem, int subclass)
{
	might_sleep();
	rwsem_acquire(&sem->dep_map, subclass, 0, _RET_IP_);

	LOCK_CONTENDED(sem, __down_write_trylock, __down_write);
}

EXPORT_SYMBOL(down_write_nested);

void up_read_non_owner(struct rw_semaphore *sem)
{
	__up_read(sem);
}

EXPORT_SYMBOL(up_read_non_owner);

#endif


