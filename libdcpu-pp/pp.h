/**

    File:       pp.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-pp

    Authors:    James Rhodes

    Description:    Declares the public API for using the preprocessor
                    inline in programs.

**/

#ifndef __DCPU_LIBDCPUPP_H
#define __DCPU_LIBDCPUPP_H

#include <dcpu.h>
#include <bstring.h>

typedef bool(*has_t)();
typedef char(*pop_t)();
typedef void(*push_t)(char);

void ppimpl(freed_bstring filename, int line, freed_bstring lang, has_t has_input, pop_t input, push_t output);

#endif
