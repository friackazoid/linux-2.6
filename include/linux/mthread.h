#ifndef _LINUX_MTHREAD_H
#define _LINUX_MTHREAD_H

#include <linux/err.h>
#include <linux/sched.h>

struct task_struct *mthread_create (int (*threadfn)(void *data),
				    void *data,
				    const char namefmt[], ...)
	__attribute__((format(printf, 3, 4)));

#define mthread_run(threadfn, data, namefmt, ...)			   \
({									   \
	struct task_struct *__k						   \
		= mthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \
	if (!IS_ERR(__k))						   \
		wake_up_process(__k);					   \
	__k;								   \
})

#endif /* _LINUX_MTHREAD_H */
