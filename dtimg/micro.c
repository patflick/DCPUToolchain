/**

    File:       micro.c

    Project:    DCPU-16 Tools
    Component:  Image Smasher

    Authors:    James Rhodes

    Description:    Implements the writer for the micro filesystem
            format.

**/

#include <stdio.h>
#include <stdlib.h>
#include <bfile.h>
#include <bstring.h>
#include <debug.h>
#include "base.h"
#include "micro.h"

#define MICRO_MAX_ENTRIES 10

size_t micro_table_addr;
size_t micro_table_count;

void micro_init(BFILE* out)
{
    // Micro format does not require anything
    // initialized or before the kernel.
    printd(LEVEL_VERBOSE, "creating DCPU-16 image with micro filesystem format.\n");
}

void micro_write_kernel(BFILE* out, BFILE* in, int count)
{
    int written = 0, i;
    uint16_t store;
    size_t pos;

    // Copy data.
    while (!bfeof(in))
    {
        bfputc(bfgetc(in), out);
        written++;
    }

    // Go back and write the filesystem
    // information if permitted.
    pos = bftell(out);
    bfseek(out, sizeof(uint16_t) * 2, SEEK_SET);
    bfiread(&store, out);
    bfseek(out, sizeof(uint16_t) * 2, SEEK_SET);
    if (store == 0xDEDE)
    {
        store = written / 2;
        bfiwrite(&store, out);
    }
    else
        printd(LEVEL_VERBOSE, "not writing end-of-kernel to 0x0002 (0x0002 is not 0xDEDE).\n");
    bfseek(out, pos, SEEK_SET);
    printd(LEVEL_VERBOSE, "wrote kernel (0x%04X words).\n", written / 2);

    // Store the position and count of the micro table.
    micro_table_addr = pos;
    micro_table_count = 0;

    // Write out 20 NULL entries (1 addr and 1 size for each entry).
    store = 0x0000;
    for (i = 0; i < MICRO_MAX_ENTRIES * 2; i++)
        bfiwrite(&store, out);
}

void micro_write_file(BFILE* out, BFILE* in, freed_bstring name)
{
    int written = 0;
    uint16_t store;
    size_t pos;

    // Check to make sure we don't have too many entries.
    if (micro_table_count >= MICRO_MAX_ENTRIES)
    {
        printd(LEVEL_WARNING, "ignoring file %s; maximum limit of %u files hit.\n", name.ref->data, MICRO_MAX_ENTRIES);
        exit(1);
    }

    // Copy data.
    store = (uint16_t)(bftell(out) / 2);
    while (!bfeof(in))
    {
        bfputc(bfgetc(in), out);
        written++;
    }

    // Store the position so we can jump back to
    // this position.
    pos = bftell(out);

    // Write the entry.
    bfseek(out, micro_table_addr + micro_table_count * 2, SEEK_SET);
    bfiwrite(&store, out);
    store = written / 2;
    bfiwrite(&store, out);
    printd(LEVEL_VERBOSE, "wrote file %s at index %u (0x%04X words).\n", name.ref->data, micro_table_count, written / 2);
    micro_table_count++;

    // Seek back again.
    bfseek(out, pos, SEEK_SET);

    // Clear memory.
    bautodestroy(name);
}

void micro_close(BFILE* out)
{
}

struct fs_writer* writer_get_micro()
{
    struct fs_writer* writer = malloc(sizeof(struct fs_writer));
    writer->init = &micro_init;
    writer->write_kernel = &micro_write_kernel;
    writer->write_file = &micro_write_file;
    writer->close = &micro_close;
    return writer;
}
