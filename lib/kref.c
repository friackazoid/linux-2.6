/*
 * kref.c - library routines for handling generic reference counted objects
 *
 * Copyright (C) 2004 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2004 IBM Corp.
 *
 * based on lib/kobject.c which was:
 * Copyright (C) 2002-2003 Patrick Mochel <mochel@osdl.org>
 *
 * This file is released under the GPLv2.
 *
 */

#include <linux/kref.h>
#include <linux/module.h>
#include <linux/syscalls.h>

/**
 * kref_set - initialize object and set refcount to requested number.
 * @kref: object in question.
 * @num: initial reference counter
 */
void kref_set(struct kref *kref, int num)
{
	atomic_set(&kref->refcount, num);
	smp_mb();
}

/**
 * kref_init - initialize object.
 * @kref: object in question.
 */
void kref_init(struct kref *kref)
{
	kref_set(kref, 1);
}

void modkref_init(struct kref *kref)
{
#define __STR(X) #X
#define STR(X) __STR(X)
//        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %0, %%ebx\n"
	         "\tmovl $"STR(__SR_modkref_init)", %%eax\n"
		 "\tint $0x80\n"
		 ::"m"(kref): "ebx", "eax");
#undef STR
#undef __STR
}
EXPORT_SYMBOL(modkref_init);

SYSCALL_DEFINE1 (modkref_init, struct kref*, kref)
{
       kref_init(kref);
       return 0;
}


/**
 * kref_get - increment refcount for object.
 * @kref: object.
 */
void kref_get(struct kref *kref)
{
	WARN_ON(!atomic_read(&kref->refcount));
	atomic_inc(&kref->refcount);
	smp_mb__after_atomic_inc();
}

void modkref_get(struct kref *kref)

{
#define __STR(X) #X
#define STR(X) __STR(X)
//        unsigned long ret;
	 __asm__ __volatile__ (
	         "\tmovl %0, %%ebx\n"
	         "\tmovl $"STR(__SR_modkref_get)", %%eax\n"
		 "\tint $0x80\n"
		 ::"m"(kref): "ebx", "eax");
	return ;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(modkref_get);

SYSCALL_DEFINE1 (modkref_get, struct kref*, kref)
{
        kref_get(kref);
	return 0;
}


/**
 * kref_put - decrement refcount for object.
 * @kref: object.
 * @release: pointer to the function that will clean up the object when the
 *	     last reference to the object is released.
 *	     This pointer is required, and it is not acceptable to pass kfree
 *	     in as this function.
 *
 * Decrement the refcount, and if 0, call release().
 * Return 1 if the object was removed, otherwise return 0.  Beware, if this
 * function returns 0, you still can not count on the kref from remaining in
 * memory.  Only use the return value if you want to see if the kref is now
 * gone, not present.
 */
int kref_put(struct kref *kref, void (*release)(struct kref *kref))
{
	WARN_ON(release == NULL);
	WARN_ON(release == (void (*)(struct kref *))kfree);

	if (atomic_dec_and_test(&kref->refcount)) {
		release(kref);
		return 1;
	}
	return 0;
}


void modkref_put(struct kref *kref,void (*release)(struct kref *kref))

{
#define __STR(X) #X
#define STR(X) __STR(X)
        int ret;
	 __asm__ __volatile__ (
	         "\tmovl %1, %%ebx\n"
		 "\tmovl %2, %%ecx\n"
	         "\tmovl $"STR(__SR_modkref_put)", %%eax\n"
		 "\tint $0x80\n"
		 "\tmovl %%eax, %0\n"
		 :"=m"(ret):"m"(kref),"m"(release): "ebx", "eax","ecx");
	return ret;
#undef STR
#undef __STR
}
EXPORT_SYMBOL(modkref_put);

SYSCALL_DEFINE2 (modkref_put, struct kref*, kref,void, (*release)(struct kref *kref))
{
        return kref_put(kref,release);
}



EXPORT_SYMBOL(kref_set);
EXPORT_SYMBOL(kref_init);
EXPORT_SYMBOL(kref_get);
EXPORT_SYMBOL(kref_put);
