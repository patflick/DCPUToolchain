/**

    File:       dcpuops.c

    Project:    DCPU-16 Tools
    Component:  Emulator

    Authors:    James Rhodes
            Aaron Miller
            Jose Manuel Diez

    Description:    Handles opcode instructions in the
            virtual machine.

**/

#define PRIVATE_VM_ACCESS

#include <stdio.h>
#include <stdlib.h>
#include <debug.h>
#include "dcpubase.h"
#include "dcpuhook.h"

#define HOOK_MAX 50

vm_hook vm_hook_list[HOOK_MAX];
uint16_t vm_hook_mode[HOOK_MAX];
void* vm_hook_userdata[HOOK_MAX];

bool vm_hook_initialized = false;

void vm_hook_fire(vm_t* vm, uint16_t pos, uint16_t mode, void* ud)
{
    uint16_t i;

    for (i = 0; i < HOOK_MAX; i += 1)
        if (vm_hook_list[i] != NULL && vm_hook_mode[i] == mode)
            vm_hook_list[i](vm, pos, ud == NULL ? vm_hook_userdata[i] : ud);
}

void vm_hook_break(vm_t* vm)
{
    vm_hook_fire(vm, 0, HOOK_ON_BREAK, NULL);
}

void vm_hook_initialize()
{
    int i;
    for (i = 0; i < HOOK_MAX; i++)
    {
        vm_hook_list[i] = NULL;
        vm_hook_mode[i] = 0;
        vm_hook_userdata[i] = NULL;
    }

    vm_hook_initialized = true;
}

uint16_t vm_hook_register(vm_t* vm, vm_hook hook, uint16_t mode, void* ud)
{
    uint16_t id = 0;

    if (!vm_hook_initialized)
        vm_hook_initialize();

    printd(LEVEL_EVERYTHING, "registering hook\n");

    while (vm_hook_list[id] != NULL && id < HOOK_MAX)
        id++;

    if (id >= HOOK_MAX)
    {
        vm_halt(vm, "unable to register hook, maximum reached!\n");
        return 0;
    }

    vm_hook_list[id] = hook;
    vm_hook_mode[id] = mode;
    vm_hook_userdata[id] = ud;
    return id;
}

void vm_hook_unregister(vm_t* vm, uint16_t id)
{
    vm_hook_list[id] = NULL;
    vm_hook_mode[id] = HOOK_ON_NONE;
    vm_hook_userdata[id] = NULL;
}
