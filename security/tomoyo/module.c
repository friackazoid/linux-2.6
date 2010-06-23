#include <linux/module.h>

#include "common.h"
#include "tomoyo.h"
#include "realpath.h"

void tomoyo_module_set_cred (struct module *module) {
	struct tomoyo_page_buffer *tmp = tomoyo_alloc (sizeof(*tmp));
	struct tomoyo_domain_info *old_domain = tomoyo_domain();
	struct tomoyo_domain_info *domain = NULL;
	const char *original_name = module->name;
	char *new_domain_name = NULL;
	struct tomoyo_path_info r;
//	int retval;

//	retval = -ENOENT;

	if (!tmp)
		goto out;

	r.name = original_name;
	tomoyo_fill_path_info (&r);

	new_domain_name = tmp->buffer;
	//if (tomoyo_is_domain_initializer(old_domain->domainname, &r, NULL))
		snprintf (new_domain_name, TOMOYO_MAX_PATHNAME_LEN + 1,
				TOMOYO_MODULE_NAME " " "/%s", original_name);
	//else
	//	snprintf(new_domain_name, TOMOYO_MAX_PATHNAME_LEN + 1,
	//			"%s %s", old_domain->domainname, original_name);

	tomoyo_update_domain_initializer_entry(TOMOYO_MODULE_NAME, original_name, false, false);

	down_read(&tomoyo_domain_list_lock);
        domain = tomoyo_find_domain(new_domain_name);
	up_read(&tomoyo_domain_list_lock);

	if (domain)
		goto done;
	//if (is_enforce)
	//        goto done;
	
	domain = tomoyo_find_or_assign_new_domain(new_domain_name,
						  old_domain->profile);

done:
        if (domain)
		goto out;
	//printk(KERN_WARNING "TOMOYO-ERROR: Domain '%s' not defined.\n",
	//       new_domain_name);
	//if (is_enforce)
	//	retval = -EPERM;
	//else
	tomoyo_set_domain_flag(old_domain, false,
	                               TOMOYO_DOMAIN_FLAGS_TRANSITION_FAILED);
out:
	if (!domain)
		domain = old_domain;
	module->security = domain;
	tomoyo_free (tmp);
	return;
}

int tomoyo_module_check_funk_perm (struct module *mod, const char* funk_name) {
	struct tomoyo_page_buffer *tmp = tomoyo_alloc (sizeof(*tmp));
	struct tomoyo_domain_info *domain = mod->security;
	struct tomoyo_single_path_acl_record *acl;
	struct tomoyo_acl_info *ptr;
	const u16 perm = 1;
	char* func_name = NULL;
	const struct tomoyo_path_info *saved_filename;
	int error = -ENOMEM;
	//struct tomoyo_path_info r;

	if (!tmp)
		goto out;
	func_name = tmp->buffer;

	snprintf (func_name, TOMOYO_MAX_PATHNAME_LEN + 1, "/%s", funk_name);

	saved_filename = tomoyo_save_name (func_name);

	if (!saved_filename)
		return error;

	down_write(&tomoyo_domain_acl_info_list_lock);

	list_for_each_entry (ptr, &domain->acl_info_list, list) {
		if (tomoyo_acl_type1(ptr) != TOMOYO_TYPE_SINGLE_PATH_ACL)
			continue;
		acl = container_of(ptr, struct tomoyo_single_path_acl_record, head);
		if (acl->filename != saved_filename)
			continue;
		goto out;
/*	if (domain->profile == 0 || domain->profile == 1) {
		//tomoyo_update_file_acl (funk_name, TOMOYO_TYPE_EXECUTE_ACL, old_domain, FALSE);
		acl = tomoyo_alloc_acl_element(TOMOYO_TYPE_SINGLE_PATH_ACL);
		if (!acl)
	                goto out;
		acl->perm = perm << TOMOYO_TYPE_EXECUTE_ACL;
		acl->filename = funk_name;
		list_add_tail(&acl->head.list, &domain->acl_info_list);
	}*/
	}

	acl = tomoyo_alloc_acl_element(TOMOYO_TYPE_SINGLE_PATH_ACL);
	if (!acl)
		goto out;
	acl->perm = perm << TOMOYO_TYPE_READ_WRITE_ACL;
	/*if (perm == (1 << TOMOYO_TYPE_READ_WRITE_ACL))
		acl->perm |= rw_mask;*/
	acl->filename = saved_filename;
	list_add_tail(&acl->head.list, &domain->acl_info_list);
	error = 0;
//	goto out;
out:
	up_write(&tomoyo_domain_acl_info_list_lock);
	tomoyo_free (tmp);
	return error;
}

