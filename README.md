# Syscall hooks

A small project of hooking the `execve()` syscall, to randomly sabotage typed bash commands.
This project was tested on `5.11.0-38-generic`.

## Usage

Before building the project, make sure to update the `sys_call_table` address in [hooks.c](./hooks.c) with the value of following command

```sh
$ sudo cat /proc/kallsyms | grep ' sys_call_table$'
```

To build and load simply execute

```sh
$ make && sudo insmod sabotage.ko
```
