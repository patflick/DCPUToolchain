///
/// @addtogroup LibDCPU-LD
/// @brief Library for performing linking.
///
/// This library manipulates DCPU-16 code according to rules and information
/// in object files to combine DCPU-16 code into a single resulting binary.
///
/// @{
///
/// @file
/// @brief API for creating and modifying sets of linker bins.
/// @author James Rhodes
/// 
/// This header provides functionality for loading, creating, re-arranging
/// and optimizing sets of linker bins.
///

#ifndef __DCPU_LD_BINS_H
#define __DCPU_LD_BINS_H

#include <stdint.h>
#include <bstring.h>
#include <lprov.h>
#include <dcpu.h>
#include <policy.h>
#include "ldbin.h"

#define OPTIMIZE_SPEED 0
#define OPTIMIZE_SIZE 1
#define OPTIMIZE_NONE 0
#define OPTIMIZE_FAST 1
#define OPTIMIZE_AGGRESSIVE 2
#define OPTIMIZE_DANGEROUS 3

///
/// @brief The global bin storage.
///
struct
{
    list_t bins;
} ldbins;

void bins_init();
struct ldbin* bins_add(freed_bstring name, struct lprov_entry* provided, struct lprov_entry* required,
                       struct lprov_entry* adjustment, struct lprov_entry* section,
                       struct lprov_entry* output, struct lprov_entry* jump,
                       struct lprov_entry* optional, struct lprov_entry* call);
void bins_set_kernel(struct lprov_entry* jump);
bool bins_load(freed_bstring path, bool loadDebugSymbols, const char* debugSymbolExtension);
bool bins_load_jumplist(freed_bstring path);
bool bins_load_kernel(freed_bstring jumplist, freed_bstring kernel);
void bins_save(freed_bstring name, freed_bstring path, int target, bool keepOutputs, const char* symbolFilename, const char* jumplistFilename);
bool bins_write(freed_bstring name, uint16_t word);
void bins_associate();
void bins_sectionize();
void bins_write_jump();
void bins_flatten(freed_bstring name);
int32_t bins_optimize(int target, int level);
void bins_resolve_kernel(policies_t* policies);
void bins_resolve(bool keepProvided, bool allowMissing, bool keepOptional);
uint16_t bins_compress();
void bins_free();

#endif

///
/// @}
///