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
#include "crypto/hmac.h"

typedef struct {
    u16 x1;
    u16 uid;
    char name[0x0C];
    u32 iblk;
    u32 ifst;
    u32 x3;
    u8 pad0[0x24];
} isfs_hmac_data;
_Static_assert(sizeof(isfs_hmac_data) == 0x40, "isfs_hmac_data size must be 0x40!");

typedef struct {
    u8 pad0[0x12];
    u16 cluster;
    u8 pad1[0x2b];
} isfs_hmac_meta;
_Static_assert(sizeof(isfs_hmac_meta) == 0x40, "isfs_hmac_meta size must be 0x40!");
