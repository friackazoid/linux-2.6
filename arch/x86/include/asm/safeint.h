#include <linux/types.h>
#include <linux/kmod.h>

void smutex_lock (struct mutex *lock);
void* skzalloc (size_t size, gfp_t flags);
void smutex_unlock (struct mutex *lock);
int skern_path (const char *name, unsigned int flags, struct path *path);
int scall_usermodehelper(char *path, char **argv, char **envp, enum umh_wait wait);
