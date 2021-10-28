#include <linux/syscalls.h>
#include <linux/printk.h>
#include <linux/types.h>

MODULE_LICENSE("GPL");

#define CR0_WRITE_PROTECT   (1 << 16)

// replace this with address in `sudo cat /proc/kallsyms | grep ' sys_call_table$'`
unsigned long *sys_call_table = (unsigned long *)0x0;

static uint64_t get_cr0(void)
{
    uint64_t ret;

    __asm__ volatile (
        "movq %%cr0, %[ret]"
        : [ret] "=r" (ret)
    );
    
    return ret;
}

static void set_cr0(uint64_t cr0)
{
    __asm__ volatile (
        "movq %[cr0], %%cr0"
        :
        : [cr0] "r" (cr0)
    );
}

asmlinkage void* hook_syscall(int syscall_number, void* replacemet) {
    uint64_t orig_cr0 = 0;
    void* original = NULL;
    
    if (sys_call_table == NULL) {
        return NULL;
    }

	orig_cr0 = get_cr0();
    set_cr0(orig_cr0 & ~CR0_WRITE_PROTECT);

    original = (void*)sys_call_table[syscall_number];
	sys_call_table[syscall_number] = (unsigned long)replacemet;

	set_cr0(orig_cr0);

    return original;
}

asmlinkage void unhook_syscall(int syscall_number, void* replacemet) {
    uint64_t orig_cr0 = 0;
    
    if (sys_call_table == NULL) {
        return;
    }

	orig_cr0 = get_cr0();
    set_cr0(orig_cr0 & ~CR0_WRITE_PROTECT);

	sys_call_table[syscall_number] = (unsigned long)replacemet;

	set_cr0(orig_cr0);
}