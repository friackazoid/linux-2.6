/* Module thread helper functions
 *   Copyright (C) 2009 Kojanov Eugeni, Roman Sergeev, Alina Stepina
 */

#include <linux/sched.h>
#include <linux/mthread.h>
#include <linux/completion.h>
#include <linux/err.h>
#include <linux/cpuset.h>
#include <linux/unistd.h>
#include <linux/file.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <trace/events/sched.h>

