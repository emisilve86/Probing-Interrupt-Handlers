# Replace Multi-Byte NOPs with CALLs to user-defined Kernel Functions

## Description

This Linux kernel module is designed to provide a new service that allows to replace multi-byte NOPs (No-Operation instructions) encountered along the execution trace of any intyerrupt handler with CALLs (function-call instructions) to functions implemented by the user within the module. This is a simple workaround/alternative solution (limited to the code involved in the handling of interrupts) for kernel images compiled without `CONFIG_KPROBES`, or for those functions that have been black-listed and therefore not-kprobable.

The `module` folder contains the source files and headers of the module itself. Within the source file `probing-function.c` you'll find instructions on how to declare your own probing functions specifically implemented for your puroposes.

The `test` folder contains an example application for using the service exposed by the module which probly provides the best example to understand how to use the service exposed by this module.

## Internals

This module basically installs a char device that can be `read` by user-space applications in order to retrieve the addresses of all NOPs that have been encountered along the execution trace of a given interrupt handler, along with all the addresses of the probing functions implemented by the user within the module. Then, the application can use such information in order to indicate that a certain NOP must be updated with a CALL to the selcted probing function. To do this, the device can be driven with apposite commands passed through `ioctl` that are:

* P_IDT_SELECT - the argument passed with this command specifies the IDT vector associated with the interrupt handler that we want to inspect in order to find NOPs (once this value is specified the application ca perform a `read` against the device)

* P_NOP_SELECT - the argument passed with this command specifies the NOP address selected (it requires that an IDT vector has already been specified)

* P_NOP_UPDATE - the argument passed with this command specifies the address of the chosen probing function (it requires that a NOP address has already been specified)

* P_NOP_RESTORE - the argument passed with this command specifies the address where a NOP was present prior to update it with a CALL so as to restore the old NOP

Opening the device (*i.e.*, invoking `open`) means starting an exclusive session to operate with it, that is, no concurrent interactions with the device are possible. The session ends when the application closes the descriptor associated with the device (*i.e.*, invoking `close`). If the module is uninstalled from the kernel before removing the CALLs inserted in place of some NOPs, then they are automatically removed upon module finalization in order not to let any reference to pages no longer mapped in kernel-space.

## Build and Install

In order to build and install this module the folowing commands are sufficient:

* `make`

* `sudo insmod probing-nop-module.ko`

To check that everything is gone ok, use `sudo dmesg` to consult the kernel log.