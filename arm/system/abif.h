/*
 *  It's a Project! linux-loader
 *
 *  Copyright (C) 2022          Ash Logan <ash@heyquark.com>
 *  Copyright (C) 2022          rw-r-r-0644 <r.r.qwertyuiop.r.r@gmail.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __ABIF_H__
#define __ABIF_H__

#include "common/types.h"

u32 abif_gpu_read32(u16 offs);
void abif_gpu_write32(u16 offs, u32 data);

void abif_gpu_setup(void);

#endif
