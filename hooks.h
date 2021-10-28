#ifndef _SYSCALL_H
#define _SYSCALL_H

void* hook_syscall(int syscall_number, void* replacemet);
void unhook_syscall(int syscall_number, void* replacemet);

#endif