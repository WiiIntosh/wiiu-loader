/*
 *  It's a Project! linux-loader
 *
 *  Copyright (C) 2022          Ash Logan <ash@heyquark.com>
 *  Copyright (C) 2022          rw-r-r-0644 <r.r.qwertyuiop.r.r@gmail.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "common/utils.h"
#include "abif.h"
#include "latte.h"
#include "latte_gpu.h"

static ALWAYS_INLINE u32 __abif_read(u32 offs) {
    write32(LT_ABIF_CPLTL_OFFSET, offs);
    return read32(LT_ABIF_CPLTL_DATA);
}

static ALWAYS_INLINE void __abif_write(u32 offs, u32 data) {
    write32(LT_ABIF_CPLTL_OFFSET, offs);
    write32(LT_ABIF_CPLTL_DATA, data);
    read32(LT_ABIF_CPLTL_DATA); // force read cycle
}

u32 abif_gpu_read32(u16 offs) {
    return (u32)__abif_read(0xc0000000 | offs);
}

void abif_gpu_write32(u16 offs, u32 data) {
    __abif_write(0xc0000000 | offs, data);
}

void abif_gpu_setup(void) {
    // Try and set up a framebuffer how Linux likes it
    abif_gpu_write32(D1GRPH + DGRPH_CONTROL, DGRPH_DEPTH_32BPP | DGRPH_FORMAT_32BPP_ARGB8888 | DGRPH_ARRAY_LINEAR_ALIGNED);
    abif_gpu_write32(D2GRPH + DGRPH_CONTROL, DGRPH_DEPTH_32BPP | DGRPH_FORMAT_32BPP_ARGB8888 | DGRPH_ARRAY_LINEAR_ALIGNED);

    // Try to account for de_Fuse weirdness
    uint32_t crossbar;
    if ((read16(LT_GPU_ENDIANNESS) & 3) != 2) {
        // de_Fuse goes here
        crossbar = DGRPH_CROSSBAR(RED, GREEN, ALPHA, BLUE); // this is wrong but I can't work out wtf Linux is doing
    } else {
        // presumably Cafe warmboot ends up here
        crossbar = DGRPH_CROSSBAR(ALPHA, BLUE, GREEN, RED);
    }

    abif_gpu_write32(D1GRPH + DGRPH_SWAP_CNTL, crossbar);
    abif_gpu_write32(D2GRPH + DGRPH_SWAP_CNTL, crossbar);
}
