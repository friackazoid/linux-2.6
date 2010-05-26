#include <linux/module.h>

#include "common.h"
#include "tomoyo.h"
#include "realpath.h"

void tomoyo_module_set_cred (struct module *module)
{
	struct tomoyo_domain_info *old_domain = tomoyo_domain();
	struct tomoyo_domain_info *domain = NULL;
	module->security = NULL;
}
