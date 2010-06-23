#ifndef __TOMOYO_MODULE_H__
#define __TOMOYO_MODULE_H__

void tomoyo_module_set_cred (struct module *module);
int tomoyo_module_check_funk_perm (struct module* mod, const char* funk_name);

#endif
