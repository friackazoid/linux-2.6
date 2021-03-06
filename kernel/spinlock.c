/*
 * Copyright (2004) Linus Torvalds
 *
 * Author: Zwane Mwaikambo <zwane@fsmlabs.com>
 *
 * Copyright (2004, 2005) Ingo Molnar
 *
 * This file contains the spinlock/rwlock implementations for the
 * SMP and the DEBUG_SPINLOCK cases. (UP-nondebug inlines them)
 *
 * Note that some architectures have special knowledge about the
 * stack frames of these functions in their profile_pc. If you
 * change anything significant here that could change the stack
 * frame contact the architecture maintainers.
 */

#include <linux/linkage.h>
#include <linux/preempt.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/debug_locks.h>
#include <linux/module.h>
#include <linux/syscalls.h>

/*
 * If lockdep is enabled then we use the non-preemption spin-ops
 * even on CONFIG_PREEMPT, because lockdep assumes that interrupts are
 * not re-enabled during lock-acquire (which the preempt-spin-ops do):
 */
#if !defined(CONFIG_GENERIC_LOCKBREAK) || defined(CONFIG_DEBUG_LOCK_ALLOC)
/*
 * The __lock_function inlines are taken from
 * include/linux/spinlock_api_smp.h
 */
#else
/*
 * We build the __lock_function inlines here. They are too large for
 * inlining all over the place, but here is only one user per function
 * which embedds them into the calling _lock_function below.
 *
 * This could be a long-held lock. We both prepare to spin for a long
 * time (making _this_ CPU preemptable if possible), and we also signal
 * towards that other CPU that it should break the lock ASAP.
 */
#define BUILD_LOCK_OPS(op, locktype)					\
void __lockfunc __##op##_lock(locktype##_t *lock)			\
{									\
	for (;;) {							\
		preempt_disable();					\
		if (likely(_raw_##op##_trylock(lock)))			\
			break;						\
		preempt_enable();					\
									\
		if (!(lock)->break_lock)				\
			(lock)->break_lock = 1;				\
		while (!op##_can_lock(lock) && (lock)->break_lock)	\
			_raw_##op##_relax(&lock->raw_lock);		\
	}								\
	(lock)->break_lock = 0;						\
}									\
									\
unsigned long __lockfunc __##op##_lock_irqsave(locktype##_t *lock)	\
{									\
	unsigned long flags;						\
									\
	for (;;) {							\
		preempt_disable();					\
		local_irq_save(flags);					\
		if (likely(_raw_##op##_trylock(lock)))			\
			break;						\
		local_irq_restore(flags);				\
		preempt_enable();					\
									\
		if (!(lock)->break_lock)				\
			(lock)->break_lock = 1;				\
		while (!op##_can_lock(lock) && (lock)->break_lock)	\
			_raw_##op##_relax(&lock->raw_lock);		\
	}								\
	(lock)->break_lock = 0;						\
	return flags;							\
}									\
									\
void __lockfunc __##op##_lock_irq(locktype##_t *lock)			\
{									\
	_##op##_lock_irqsave(lock);					\
}									\
									\
void __lockfunc __##op##_lock_bh(locktype##_t *lock)			\
{									\
	unsigned long flags;						\
									\
	/*							*/	\
	/* Careful: we must exclude softirqs too, hence the	*/	\
	/* irq-disabling. We use the generic preemption-aware	*/	\
	/* function:						*/	\
	/**/								\
	flags = _##op##_lock_irqsave(lock);				\
	local_bh_disable();						\
	local_irq_restore(flags);					\
}									\

/*
 * Build preemption-friendly versions of the following
 * lock-spinning functions:
 *
 *         __[spin|read|write]_lock()
 *         __[spin|read|write]_lock_irq()
 *         __[spin|read|write]_lock_irqsave()
 *         __[spin|read|write]_lock_bh()
 */
BUILD_LOCK_OPS(spin, spinlock);
BUILD_LOCK_OPS(read, rwlock);
BUILD_LOCK_OPS(write, rwlock);

#endif

#ifdef CONFIG_DEBUG_LOCK_ALLOC

void __lockfunc _spin_lock_nested(spinlock_t *lock, int subclass)
{
	preempt_disable();
	spin_acquire(&lock->dep_map, subclass, 0, _RET_IP_);
	LOCK_CONTENDED(lock, _raw_spin_trylock, _raw_spin_lock);
}
EXPORT_SYMBOL(_spin_lock_nested);

unsigned long __lockfunc _spin_lock_irqsave_nested(spinlock_t *lock,
						   int subclass)
{
	unsigned long flags;

	local_irq_save(flags);
	preempt_disable();
	spin_acquire(&lock->dep_map, subclass, 0, _RET_IP_);
	LOCK_CONTENDED_FLAGS(lock, _raw_spin_trylock, _raw_spin_lock,
				_raw_spin_lock_flags, &flags);
	return flags;
}
EXPORT_SYMBOL(_spin_lock_irqsave_nested);

void __lockfunc _spin_lock_nest_lock(spinlock_t *lock,
				     struct lockdep_map *nest_lock)
{
	preempt_disable();
	spin_acquire_nest(&lock->dep_map, 0, 0, nest_lock, _RET_IP_);
	LOCK_CONTENDED(lock, _raw_spin_trylock, _raw_spin_lock);
}
EXPORT_SYMBOL(_spin_lock_nest_lock);

#endif

#ifndef CONFIG_INLINE_SPIN_TRYLOCK
int __lockfunc _spin_trylock(spinlock_t *lock)
{
	return __spin_trylock(lock);
}
EXPORT_SYMBOL(_spin_trylock);
#endif

#ifndef CONFIG_INLINE_READ_TRYLOCK
int __lockfunc _read_trylock(rwlock_t *lock)
{
	return __read_trylock(lock);
}
EXPORT_SYMBOL(_read_trylock);
#endif

#ifndef CONFIG_INLINE_WRITE_TRYLOCK
int __lockfunc _write_trylock(rwlock_t *lock)
{
	return __write_trylock(lock);
}
EXPORT_SYMBOL(_write_trylock);
#endif

#ifndef CONFIG_INLINE_READ_LOCK
void __lockfunc _read_lock(rwlock_t *lock)
{
	__read_lock(lock);
}
EXPORT_SYMBOL(_read_lock);
#endif

#ifndef CONFIG_INLINE_SPIN_LOCK_IRQSAVE
unsigned long __lockfunc _spin_lock_irqsave(spinlock_t *lock)
{
	return __spin_lock_irqsave(lock);
}
EXPORT_SYMBOL(_spin_lock_irqsave);
unsigned long __lockfunc mod_spin_lock_irqsave(spinlock_t *lock)
{
#define __STR(X) #X
#define STR(X) __STR(X)
	unsigned long ret;	
	__asm__ __volatile__ (
		"\tmovl %1, %%ebx\n"
		"\tmovl $"STR(__SR_mod_spin_lock_irqsave)", %%eax\n"
		"\tint $0x80\n"
		"\tmovl %%eax, %0"
		:"=m" (ret):"m"(lock):"ebx", "eax");
	return ret;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(mod_spin_lock_irqsave);
SYSCALL_DEFINE1(mod_spin_lock_irqsave, spinlock_t, *lock)
{
	return _spin_lock_irqsave(lock);
}
#endif

#ifndef CONFIG_INLINE_SPIN_LOCK_IRQ
void __lockfunc _spin_lock_irq(spinlock_t *lock)
{
	__spin_lock_irq(lock);
}
EXPORT_SYMBOL(_spin_lock_irq);

void __lockfunc mod_spin_lock_irq(spinlock_t *lock)
{
#define __STR(X) #X
#define STR(X) __STR(X)
		
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_spin_lock_irq)", %%eax\n"
		"\tint $0x80\n"
		::"m"(lock) :"ebx", "eax");

#undef STR
#undef __STR
}
EXPORT_SYMBOL(mod_spin_lock_irq);
SYSCALL_DEFINE1(mod_spin_lock_irq, spinlock_t, *lock)
{
	_spin_lock_irq(lock);
	return;
}
#endif

#ifndef CONFIG_INLINE_SPIN_LOCK_BH
void __lockfunc _spin_lock_bh(spinlock_t *lock)
{
	__spin_lock_bh(lock);
}
EXPORT_SYMBOL(_spin_lock_bh);
#endif

#ifndef CONFIG_INLINE_READ_LOCK_IRQSAVE
unsigned long __lockfunc _read_lock_irqsave(rwlock_t *lock)
{
	return __read_lock_irqsave(lock);
}
EXPORT_SYMBOL(_read_lock_irqsave);
#endif

#ifndef CONFIG_INLINE_READ_LOCK_IRQ
void __lockfunc _read_lock_irq(rwlock_t *lock)
{
	__read_lock_irq(lock);
}
EXPORT_SYMBOL(_read_lock_irq);
#endif

#ifndef CONFIG_INLINE_READ_LOCK_BH
void __lockfunc _read_lock_bh(rwlock_t *lock)
{
	__read_lock_bh(lock);
}
EXPORT_SYMBOL(_read_lock_bh);
#endif

#ifndef CONFIG_INLINE_WRITE_LOCK_IRQSAVE
unsigned long __lockfunc _write_lock_irqsave(rwlock_t *lock)
{
	return __write_lock_irqsave(lock);
}
EXPORT_SYMBOL(_write_lock_irqsave);
#endif

#ifndef CONFIG_INLINE_WRITE_LOCK_IRQ
void __lockfunc _write_lock_irq(rwlock_t *lock)
{
	__write_lock_irq(lock);
}
EXPORT_SYMBOL(_write_lock_irq);
#endif

#ifndef CONFIG_INLINE_WRITE_LOCK_BH
void __lockfunc _write_lock_bh(rwlock_t *lock)
{
	__write_lock_bh(lock);
}
EXPORT_SYMBOL(_write_lock_bh);
#endif

#ifndef CONFIG_INLINE_SPIN_LOCK
void __lockfunc _spin_lock(spinlock_t *lock)
{
	__spin_lock(lock);
}
EXPORT_SYMBOL(_spin_lock);

void __lockfunc mod_spin_lock(spinlock_t *lock)
{
#define __STR(X) #X
#define STR(X) __STR(X)	
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl $"STR(__SR_mod_spin_lock)", %%eax\n"
		"\tint $0x80\n"
		::"m"(lock) :"ebx", "eax");
#undef STR
#undef __STR
}
EXPORT_SYMBOL(mod_spin_lock);
SYSCALL_DEFINE1(mod_spin_lock, spinlock_t, *lock)
{
	_spin_lock(lock);
	return;
}
#endif

#ifndef CONFIG_INLINE_WRITE_LOCK
void __lockfunc _write_lock(rwlock_t *lock)
{
	__write_lock(lock);
}
EXPORT_SYMBOL(_write_lock);
#endif

#ifndef CONFIG_INLINE_SPIN_UNLOCK
void __lockfunc _spin_unlock(spinlock_t *lock)
{
	__spin_unlock(lock);
}
EXPORT_SYMBOL(_spin_unlock);
#endif

#ifndef CONFIG_INLINE_WRITE_UNLOCK
void __lockfunc _write_unlock(rwlock_t *lock)
{
	__write_unlock(lock);
}
EXPORT_SYMBOL(_write_unlock);
#endif

#ifndef CONFIG_INLINE_READ_UNLOCK
void __lockfunc _read_unlock(rwlock_t *lock)
{
	__read_unlock(lock);
}
EXPORT_SYMBOL(_read_unlock);
#endif

#ifndef CONFIG_INLINE_SPIN_UNLOCK_IRQRESTORE
void __lockfunc _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
{
	__spin_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(_spin_unlock_irqrestore);
void __lockfunc mod_spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
{
#define __STR(X) #X
#define STR(X) __STR(X)
		
	__asm__ __volatile__ (
		"\tmovl %0, %%ebx\n"
		"\tmovl %1, %%ecx\n"
		"\tmovl $"STR(__SR_mod_spin_unlock_irqrestore)", %%eax\n"
		"\tint $0x80\n"
		::"m"(lock), "m"(flags) :"ebx", "eax");

#undef STR
#undef __STR
}
EXPORT_SYMBOL(mod_spin_unlock_irqrestore);
SYSCALL_DEFINE2(mod_spin_unlock_irqrestore, spinlock_t, *lock, unsigned long, flags)
{
	_spin_unlock_irqrestore(lock, flags);
	return;
}
#endif

#ifndef CONFIG_INLINE_SPIN_UNLOCK_IRQ
void __lockfunc _spin_unlock_irq(spinlock_t *lock)
{
	__spin_unlock_irq(lock);
}
EXPORT_SYMBOL(_spin_unlock_irq);
#endif

#ifndef CONFIG_INLINE_SPIN_UNLOCK_BH
void __lockfunc _spin_unlock_bh(spinlock_t *lock)
{
	__spin_unlock_bh(lock);
}
EXPORT_SYMBOL(_spin_unlock_bh);
#endif

#ifndef CONFIG_INLINE_READ_UNLOCK_IRQRESTORE
void __lockfunc _read_unlock_irqrestore(rwlock_t *lock, unsigned long flags)
{
	__read_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(_read_unlock_irqrestore);
#endif

#ifndef CONFIG_INLINE_READ_UNLOCK_IRQ
void __lockfunc _read_unlock_irq(rwlock_t *lock)
{
	__read_unlock_irq(lock);
}
EXPORT_SYMBOL(_read_unlock_irq);
#endif

#ifndef CONFIG_INLINE_READ_UNLOCK_BH
void __lockfunc _read_unlock_bh(rwlock_t *lock)
{
	__read_unlock_bh(lock);
}
EXPORT_SYMBOL(_read_unlock_bh);
#endif

#ifndef CONFIG_INLINE_WRITE_UNLOCK_IRQRESTORE
void __lockfunc _write_unlock_irqrestore(rwlock_t *lock, unsigned long flags)
{
	__write_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(_write_unlock_irqrestore);
#endif

#ifndef CONFIG_INLINE_WRITE_UNLOCK_IRQ
void __lockfunc _write_unlock_irq(rwlock_t *lock)
{
	__write_unlock_irq(lock);
}
EXPORT_SYMBOL(_write_unlock_irq);
#endif

#ifndef CONFIG_INLINE_WRITE_UNLOCK_BH
void __lockfunc _write_unlock_bh(rwlock_t *lock)
{
	__write_unlock_bh(lock);
}
EXPORT_SYMBOL(_write_unlock_bh);
#endif

#ifndef CONFIG_INLINE_SPIN_TRYLOCK_BH
int __lockfunc _spin_trylock_bh(spinlock_t *lock)
{
	return __spin_trylock_bh(lock);
}
EXPORT_SYMBOL(_spin_trylock_bh);
#endif

notrace int in_lock_functions(unsigned long addr)
{
	/* Linker adds these: start and end of __lockfunc functions */
	extern char __lock_text_start[], __lock_text_end[];

	return addr >= (unsigned long)__lock_text_start
	&& addr < (unsigned long)__lock_text_end;
}
EXPORT_SYMBOL(in_lock_functions);