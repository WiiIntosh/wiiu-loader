/*
 *  It's a Project! linux-loader
 *
 *  Copyright (C) 2022          Ash Logan <ash@heyquark.com>
 *  Copyright (C) 2022          rw-r-r-0644 <r.r.qwertyuiop.r.r@gmail.com>
 *
 *  Based on AMD RV630 Reference Guide
 *  Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#define D1GRPH 0x6100
#define D2GRPH 0x6900

#define DGRPH_ENABLE 0x0000
    #define DGRPH_ENABLE_REG 0x1
#define DGRPH_CONTROL 0x0004
    #define DGRPH_DEPTH 0x3
        #define DGRPH_DEPTH_8BPP 0x0
        #define DGRPH_DEPTH_16BPP 0x1
        #define DGRPH_DEPTH_32BPP 0x2
        #define DGRPH_DEPTH_64BPP 0x3
    #define DGRPH_FORMAT 0x700
        #define DGRPH_FORMAT_8BPP_INDEXED 0x000
        #define DGRPH_FORMAT_16BPP_ARGB1555 0x000
        #define DGRPH_FORMAT_16BPP_RGB565 0x100
        #define DGRPH_FORMAT_16BPP_ARGB4444 0x200
        //todo: 16bpp alpha index 88, mono 16, brga 5551
        #define DGRPH_FORMAT_32BPP_ARGB8888 0x000
        #define DGRPH_FORMAT_32BPP_ARGB2101010 0x100
        #define DGRPH_FORMAT_32BPP_DIGITAL 0x200
        #define DGRPH_FORMAT_32BPP_8ARGB2101010 0x300
        #define DGRPH_FORMAT_32BPP_BGRA1010102 0x400
        #define DGRPH_FORMAT_32BPP_8BGRA1010102 0x500
        #define DGRPH_FORMAT_32BPP_RGB111110 0x600
        #define DGRPH_FORMAT_32BPP_BGR101111 0x700
        //todo: 64bpp
    #define DGRPH_ADDRESS_TRANSLATION 0x10000
        #define DGRPH_ADDRESS_TRANSLATION_PHYS 0x00000
        #define DGRPH_ADDRESS_TRANSLATION_VIRT 0x10000
    #define DGRPH_PRIVILEGED_ACCESS 0x20000
        #define DGRPH_PRIVILEGED_ACCESS_DISABLE 0x00000
        #define DGRPH_PRIVILEGED_ACCESS_ENABLE 0x20000
    #define DGRPH_ARRAY_MODE 0xF00000
        #define DGRPH_ARRAY_LINEAR_GENERAL 0x000000
        #define DGRPH_ARRAY_LINEAR_ALIGNED 0x100000
        #define DGRPH_ARRAY_1D_TILES_THIN1 0x200000
        //todo: rest of these array modes

#define DGRPH_SWAP_CNTL 0x000C
    #define DGRPH_ENDIAN_SWAP 0x3
        #define DGRPH_ENDIAN_SWAP_NONE 0x0
        #define DGRPH_ENDIAN_SWAP_16 0x1
        #define DGRPH_ENDIAN_SWAP_32 0x2
        #define DGRPH_ENDIAN_SWAP_64 0x3
    #define DGRPH_RED_CROSSBAR 0x30
        #define DGRPH_RED_CROSSBAR_RED 0x00
    #define DGRPH_GREEN_CROSSBAR 0xC0
        #define DGRPH_GREEN_CROSSBAR_GREEN 0x00
    #define DGRPH_BLUE_CROSSBAR 0x300
        #define DGRPH_BLUE_CROSSBAR_BLUE 0x000
    #define DGRPH_ALPHA_CROSSBAR 0xC00
        #define DGRPH_ALPHA_CROSSBAR_ALPHA 0x000
    // todo: other values for crossbars

#define DGRPH_PRIMARY_SURFACE_ADDRESS 0x0010
    #define DGRPH_PRIMARY_DFQ_ENABLE 0x1
        #define DGRPH_PRIMARY_DFQ_OFF 0x0
        #define DGRPH_PRIMARY_DFQ_ON 0x1
    #define DGRPH_PRIMARY_SURFACE_ADDR 0xFFFFFF00

#define DGRPH_PITCH 0x0020
    #define DGRPH_PITCH_VAL 0x3FFF

#define DGRPH_SURFACE_OFFSET_X 0x0024
    #define DGRPH_SURFACE_OFFSET_X_VAL 0x1F00

#define DGRPH_SURFACE_OFFSET_Y 0x0028
    #define DGRPH_SURFACE_OFFSET_Y_VAL 0x1FFE

#define DGRPH_X_START 0x002C
    #define DGRPH_X_START_VAL 0x1FFF

#define DGRPH_Y_START 0x0030
    #define DGRPH_Y_START_VAL 0x1FFF

#define DGRPH_X_END 0x0034
    #define DGRPH_X_END_VAL 0x3FFF

#define DGRPH_Y_END 0x0038
    #define DGRPH_Y_END_VAL 0x3FFF
