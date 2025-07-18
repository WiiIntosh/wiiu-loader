/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef _GFX_H
#define _GFX_H

#include "common/types.h"

#define RED    (0xFFFF0000)
#define GREEN  (0xFF00FF00)
#define BLACK  (0x00000000)
#define WHITE  (0xFFFFFFFF)

typedef enum {
	GFX_TV = 0,
	GFX_DRC,

	GFX_ALL,
} gfx_screen_t;

void gfx_init(void);
void gfx_draw_plot(gfx_screen_t screen, int x, int y, u32 color);
void gfx_clear(gfx_screen_t screen, u32 color);
void gfx_draw_string(gfx_screen_t screen, const char* str, int x, int y, u32 color);
void gfx_draw_char(gfx_screen_t screen, char c, int x, int y, u32 color);
void gfx_draw_logo(gfx_screen_t screen);
int printf(const char* fmt, ...);
void sram_print(const char* msg);
void sram_print_hex(u32 hex);
#define spr(str) sram_print(SRAM_STR(str))
#define spc(ptr) sram_print(ptr)
#define sph(hex) sram_print_hex((u32)(hex))
//for single chars (newlines, spaces) this helps reduce duplicate .rodata stuff
#define spch(chr) {char str[2] = {chr, '\0'}; spc(str);}
#define sphs(hex) {sph(hex);spch(' ');}

#endif
