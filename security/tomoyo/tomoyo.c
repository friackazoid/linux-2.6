/*
 * security/tomoyo/tomoyo.c
 *
 * LSM hooks for TOMOYO Linux.
 *
 * Copyright (C) 2005-2009  NTT DATA CORPORATION
 *
 * Version: 2.2.0   2009/04/01
 *
 */

#include <linux/security.h>
#include "common.h"
#include "tomoyo.h"
#include "realpath.h"
#include "module.h"

static int tomoyo_cred_alloc_blank(struct cred *new, gfp_t gfp)
{
	new->security = NULL;
	return 0;
}

static int tomoyo_cred_prepare(struct cred *new, const struct cred *old,
			       gfp_t gfp)
{
	/*
	 * Since "struct tomoyo_domain_info *" is a sharable pointer,
	 * we don't need to duplicate.
	 */
	new->security = old->security;
	return 0;
}

static void tomoyo_cred_transfer(struct cred *new, const struct cred *old)
{
	/*
	 * Since "struct tomoyo_domain_info *" is a sharable pointer,
	 * we don't need to duplicate.
	 */
	new->security = old->security;
}

static int tomoyo_bprm_set_creds(struct linux_binprm *bprm)
{
	int rc;

	rc = cap_bprm_set_creds(bprm);
	if (rc)
		return rc;

	/*
	 * Do only if this function is called for the first time of an execve
	 * operation.
	 */
	if (bprm->cred_prepared)
		return 0;
	/*
	 * Load policy if /sbin/tomoyo-init exists and /sbin/init is requested
	 * for the first time.
	 */
	if (!tomoyo_policy_loaded)
		tomoyo_load_policy(bprm->filename);
	/*
	 * Tell tomoyo_bprm_check_security() is called for the first time of an
	 * execve operation.
	 */
	bprm->cred->security = NULL;
	return 0;
}

/*static int tomoyo_task_create (unsigned long clone_flags)
{
	tomoyo_domain_info *domain = domoyo_domain ();
	tomoyo_path_info *path;
	tomoyo_acl_info *ptr;

	switch (domain.profile) {
		case 0:
			return 0;
		case 1:
			if(!strcmp(domain->domainname, TOMOYO_ADMINISTRATOR_NAME)) {
				down_read (&tomoyo_domain_acl_info_list_lock);
				list_for_each_entry(ptr, domain->acl_info_list, list) {
					struct 
				}
				up_read (&tomoyo_domain_acl_list_lock);
			}
	}
}*/

static int tomoyo_bprm_check_security(struct linux_binprm *bprm)
{
	struct tomoyo_domain_info *domain = bprm->cred->security;

	/*
	 * Execute permission is checked against pathname passed to do_execve()
	 * using current domain.
	 */
	if (!domain)
		return tomoyo_find_next_domain(bprm);
	/*
	 * Read permission is checked against interpreters using next domain.
	 * '1' is the result of open_to_namei_flags(O_RDONLY).
	 */
	return tomoyo_check_open_permission(domain, &bprm->file->f_path, 1);
}

static int tomoyo_path_truncate(struct path *path, loff_t length,
				unsigned int time_attrs)
{
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_TRUNCATE_ACL,
				       path);
}

static int tomoyo_path_unlink(struct path *parent, struct dentry *dentry)
{
	struct path path = { parent->mnt, dentry };
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_UNLINK_ACL,
				       &path);
}

static int tomoyo_path_mkdir(struct path *parent, struct dentry *dentry,
			     int mode)
{
	struct path path = { parent->mnt, dentry };
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_MKDIR_ACL,
				       &path);
}

static int tomoyo_path_rmdir(struct path *parent, struct dentry *dentry)
{
	struct path path = { parent->mnt, dentry };
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_RMDIR_ACL,
				       &path);
}

static int tomoyo_path_symlink(struct path *parent, struct dentry *dentry,
			       const char *old_name)
{
	struct path path = { parent->mnt, dentry };
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_SYMLINK_ACL,
				       &path);
}

static int tomoyo_path_mknod(struct path *parent, struct dentry *dentry,
			     int mode, unsigned int dev)
{
	struct path path = { parent->mnt, dentry };
	int type = TOMOYO_TYPE_CREATE_ACL;

	switch (mode & S_IFMT) {
	case S_IFCHR:
		type = TOMOYO_TYPE_MKCHAR_ACL;
		break;
	case S_IFBLK:
		type = TOMOYO_TYPE_MKBLOCK_ACL;
		break;
	case S_IFIFO:
		type = TOMOYO_TYPE_MKFIFO_ACL;
		break;
	case S_IFSOCK:
		type = TOMOYO_TYPE_MKSOCK_ACL;
		break;
	}
	return tomoyo_check_1path_perm(tomoyo_domain(),
				       type, &path);
}

static int tomoyo_path_link(struct dentry *old_dentry, struct path *new_dir,
			    struct dentry *new_dentry)
{
	struct path path1 = { new_dir->mnt, old_dentry };
	struct path path2 = { new_dir->mnt, new_dentry };
	return tomoyo_check_2path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_LINK_ACL,
				       &path1, &path2);
}

static int tomoyo_path_rename(struct path *old_parent,
			      struct dentry *old_dentry,
			      struct path *new_parent,
			      struct dentry *new_dentry)
{
	struct path path1 = { old_parent->mnt, old_dentry };
	struct path path2 = { new_parent->mnt, new_dentry };
	return tomoyo_check_2path_perm(tomoyo_domain(),
				       TOMOYO_TYPE_RENAME_ACL,
				       &path1, &path2);
}

static int tomoyo_file_fcntl(struct file *file, unsigned int cmd,
			     unsigned long arg)
{
	if (cmd == F_SETFL && ((arg ^ file->f_flags) & O_APPEND))
		return tomoyo_check_rewrite_permission(tomoyo_domain(), file);
	return 0;
}

static int tomoyo_dentry_open(struct file *f, const struct cred *cred)
{
	int flags = f->f_flags;

	if ((flags + 1) & O_ACCMODE)
		flags++;
	flags |= f->f_flags & (O_APPEND | O_TRUNC);
	/* Don't check read permission here if called from do_execve(). */
	if (current->in_execve)
		return 0;
	return tomoyo_check_open_permission(tomoyo_domain(), &f->f_path, flags);
}

/*
 * tomoyo_security_ops is a "struct security_operations" which is used for
 * registering TOMOYO.
 */
static struct security_operations tomoyo_security_ops = {
	.name                = "tomoyo",
	.cred_alloc_blank    = tomoyo_cred_alloc_blank,
	.cred_prepare        = tomoyo_cred_prepare,
	.cred_transfer	     = tomoyo_cred_transfer,
	.bprm_set_creds      = tomoyo_bprm_set_creds,
	.bprm_check_security = tomoyo_bprm_check_security,
//	.task_create	     = tomoyo_task_create,
	.file_fcntl          = tomoyo_file_fcntl,
	.dentry_open         = tomoyo_dentry_open,
	.path_truncate       = tomoyo_path_truncate,
	.path_unlink         = tomoyo_path_unlink,
	.path_mkdir          = tomoyo_path_mkdir,
	.path_rmdir          = tomoyo_path_rmdir,
	.path_symlink        = tomoyo_path_symlink,
	.path_mknod          = tomoyo_path_mknod,
	.path_link           = tomoyo_path_link,
	.path_rename         = tomoyo_path_rename,
	.module_set_cred     = tomoyo_module_set_cred,
	.module_check_funk_perm = tomoyo_module_check_funk_perm
};

static int __init tomoyo_init(void)
{
	struct cred *cred = (struct cred *) current_cred();

	if (!security_module_enable(&tomoyo_security_ops))
		return 0;
	/* register ourselves with the security framework */
	if (register_security(&tomoyo_security_ops))
		panic("Failure registering TOMOYO Linux");
//	printk(KERN_INFO "TOMOYO Linux initialized\n");
	cred->security = &tomoyo_kernel_domain;
	tomoyo_realpath_init();
	return 0;
}

security_initcall(tomoyo_init);
