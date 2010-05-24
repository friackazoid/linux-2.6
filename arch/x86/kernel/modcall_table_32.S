ENTRY(mod_call_table)
	.long sys_mod_rettoker
	.long sys_mod_printk
	.long sys_mod_mutex_lock
	.long sys_mod_kzalloc
	.long sys_mod_mutex_unlock
	.long sys_mod_kern_path
	.long sys_mod_call_usermodehelper
	.long sys_mod_kfree
	.long sys_mod_mutex_init
	.long sys_mod_securityfs_create_dir
	.long sys_mod_securityfs_create_file
