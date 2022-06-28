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

#pragma once
#include "isfs.h"
#include "storage/nand/nand.h"

#define ISFSVOL_SLC             0
#define ISFSVOL_SLCCMPT         1

#define ISFSVOL_FLAG_HMAC       1
#define ISFSVOL_FLAG_ENCRYPTED  2
#define ISFSVOL_FLAG_READBACK   4

#define ISFSVOL_OK              0
#define ISFSVOL_ECC_CORRECTED   0x10
#define ISFSVOL_HMAC_PARTIAL    0x20
#define ISFSVOL_ERROR_WRITE     -0x10
#define ISFSVOL_ERROR_READ      -0x20
#define ISFSVOL_ERROR_ERASE     -0x30
#define ISFSVOL_ERROR_HMAC      -0x40
#define ISFSVOL_ERROR_READBACK  -0x50

int isfs_num_volumes(void);
isfs_ctx* isfs_get_volume(int volume);
char* isfs_do_volume(const char* path, isfs_ctx** ctx);

int isfs_read_volume(const isfs_ctx* ctx, u32 start_cluster, u32 cluster_count, u32 flags, void *hmac_seed, void *data);
#if NAND_WRITE_ENABLED
int isfs_write_volume(const isfs_ctx* ctx, u32 start_cluster, u32 cluster_count, u32 flags, void *hmac_seed, void *data);
#endif
