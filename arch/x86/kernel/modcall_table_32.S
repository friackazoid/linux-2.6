ENTRY(mod_call_table)
	.long sys_mod_rettoker
	.long sys_mod_printk
	.long sys_mod_mutex_lock
	.long sys_mod_kzalloc
	.long sys_mod_mutex_unlock
	.long sys_mod_kern_path
	.long sys_mod_call_usermodehelper
