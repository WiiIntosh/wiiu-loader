/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2022          rw-r-r-0644 <r.r.qwertyuiop.r.r@gmail.com>
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "common/types.h"
#include "common/utils.h"
#include "video/gfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "storage/nand/nand.h"
#include "isfs.h"
#include "volume.h"
#include "super.h"
#include "dotab.h"

static bool initialized = false;

int isfs_init(void)
{
    isfs_ctx *ctx;
    int res = 0;

    if(initialized) return res;

    for(int i = 0; i < isfs_num_volumes(); i++) {
        ctx = isfs_get_volume(i);

        if (!isfs_load_super(ctx, 0, ctx->max_generation)) {
            ctx->mounted = true;
            isfs_add_dotab(ctx);
        } else
            printf("isfs: %s: failed to load superblock\n", ctx->name);
    }

    initialized = true;
    return 0;
}

int isfs_fini(void)
{
    isfs_ctx *ctx;
    if(!initialized) return 0;

    for(int i = 0; i < isfs_num_volumes(); i++) {
        ctx = isfs_get_volume(i);
        isfs_remove_dotab(ctx);
        ctx->mounted = false;
    }

    initialized = false;
    return 0;
}

isfs_fst* isfs_stat(const char* path)
{
    isfs_ctx* ctx = NULL;
    path = isfs_do_volume(path, &ctx);
    if(!ctx || !path) return NULL;

    return isfs_find_fst(ctx, NULL, path);
}

int isfs_open(isfs_file* file, const char* path)
{
    if(!file || !path) return -1;

    isfs_ctx* ctx = NULL;
    path = isfs_do_volume(path, &ctx);
    if(!ctx) return -2;

    isfs_fst* fst = isfs_find_fst(ctx, NULL, path);
    if(!fst) return -3;

    if(!isfs_fst_is_file(fst)) return -4;

    memset(file, 0, sizeof(isfs_file));
    file->volume = ctx->volume;
    file->fst = fst;

    file->cluster = fst->sub;
    file->offset = 0;

    return 0;
}

int isfs_close(isfs_file* file)
{
    if(!file) return -1;
    memset(file, 0, sizeof(isfs_file));

    return 0;
}

int isfs_seek(isfs_file* file, s32 offset, int whence)
{
    if(!file) return -1;

    isfs_ctx* ctx = isfs_get_volume(file->volume);
    isfs_fst* fst = file->fst;
    if(!ctx || !fst) return -2;

    switch(whence) {
        case SEEK_SET:
            if(offset < 0) return -1;
            if(offset > fst->size) return -1;
            file->offset = offset;
            break;

        case SEEK_CUR:
            if(file->offset + offset > fst->size) return -1;
            if((offset + (s32)fst->size) < 0) return -1;
            file->offset += offset;
            break;

        case SEEK_END:
            if(file->offset + offset > fst->size) return -1;
            if((offset + (s32)fst->size) < 0) return -1;
            file->offset = fst->size + offset;
            break;
    }

    u16 sub = fst->sub;
    size_t size = file->offset;

    while(size > 8 * PAGE_SIZE) {
        sub = isfs_get_fat(ctx)[sub];
        size -= 8 * PAGE_SIZE;
    }

    file->cluster = sub;

    return 0;
}

int isfs_read(isfs_file* file, void* buffer, size_t size, size_t* bytes_read)
{
    if(!file || !buffer) return -1;

    isfs_ctx* ctx = isfs_get_volume(file->volume);
    isfs_fst* fst = file->fst;
    if(!ctx || !fst) return -2;

    if(size + file->offset > fst->size)
        size = fst->size - file->offset;

    size_t total = size;

    void* cluster_buf = memalign(NAND_DATA_ALIGN, CLUSTER_SIZE);
    if(!cluster_buf) return -3;

    while(size) {
        size_t pos = file->offset % CLUSTER_SIZE;
        size_t copy = CLUSTER_SIZE - pos;
        if(copy > size) copy = size;

        if (isfs_read_volume(ctx, file->cluster, 1, ISFSVOL_FLAG_ENCRYPTED, NULL, cluster_buf) < 0)
        {
            free(cluster_buf);
            return -4;
        }
        memcpy(buffer, cluster_buf + pos, copy);

        file->offset += copy;
        buffer += copy;
        size -= copy;

        if((pos + copy) >= CLUSTER_SIZE)
            file->cluster = isfs_get_fat(ctx)[file->cluster];
    }

    free(cluster_buf);

    *bytes_read = total;
    return 0;
}

int isfs_diropen(isfs_dir* dir, const char* path)
{
    if(!dir || !path) return -1;

    isfs_ctx* ctx = NULL;
    path = isfs_do_volume(path, &ctx);
    if(!ctx) return -2;

    isfs_fst* fst = isfs_find_fst(ctx, NULL, path);
    if(!fst) return -3;

    if(!isfs_fst_is_dir(fst)) return -4;
    if(fst->sub == 0xFFFF) return -2;

    isfs_fst* root = isfs_get_fst(ctx);

    memset(dir, 0, sizeof(isfs_dir));
    dir->volume = ctx->volume;
    dir->dir = fst;
    dir->child = &root[fst->sub];

    return 0;
}

int isfs_dirread(isfs_dir* dir, isfs_fst** info)
{
    if(!dir) return -1;

    isfs_ctx* ctx = isfs_get_volume(dir->volume);
    isfs_fst* fst = dir->dir;
    if(!ctx || !fst) return -2;

    isfs_fst* root = isfs_get_fst(ctx);

    if(!info) {
        dir->child = &root[fst->sub];
        return 0;
    }

    *info = dir->child;

    if(dir->child != NULL) {
        if(dir->child->sib == 0xFFFF)
            dir->child = NULL;
        else
            dir->child = &root[dir->child->sib];
    }

    return 0;
}

int isfs_dirreset(isfs_dir* dir)
{
    return isfs_dirread(dir, NULL);
}

int isfs_dirclose(isfs_dir* dir)
{
    if(!dir) return -1;
    memset(dir, 0, sizeof(isfs_dir));

    return 0;
}
