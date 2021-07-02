/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "gfx.h"
#include "lw-logo.h"
#include <stdio.h>

extern const u8 msx_font[];

#define CHAR_SIZE_X (8)
#define CHAR_SIZE_Y (8)

struct {
	u32* ptr;
	int width;
	int height;
	size_t bpp;

	int current_y;
	int current_x;
} fbs[GFX_ALL] SRAM_DATA = {
	[GFX_TV] =
	{
		.ptr = (u32*)(0x14000000 + 0x3500000),
		.width = 1280,
		.height = 720,
		.bpp = 4,

		.current_y = 10,
		.current_x = 10,
	},
	[GFX_DRC] =
	{
		.ptr = (u32*)(0x14000000 + 0x38C0000),
		.width = 896,
		.height = 504,
		.bpp = 4,

		.current_y = 10,
		.current_x = 10,
	},
};

size_t SRAM_TEXT gfx_get_stride(gfx_screen_t screen)
{
	if(screen == GFX_ALL) return 0;

	return fbs[screen].bpp * fbs[screen].width;
}

size_t SRAM_TEXT gfx_get_size(gfx_screen_t screen)
{
	if(screen == GFX_ALL) return 0;

	return gfx_get_stride(screen) * fbs[screen].height;
}

void SRAM_TEXT gfx_draw_plot(gfx_screen_t screen, int x, int y, u32 color)
{
	if(screen == GFX_ALL) {
		for(int i = 0; i < GFX_ALL; i++)
			gfx_draw_plot(i, x, y, color);
	} else {
	    u32* fb = &fbs[screen].ptr[x + y * (gfx_get_stride(screen) / sizeof(u32))];
	    *fb = color;
	}
}

void SRAM_TEXT gfx_clear(gfx_screen_t screen, u32 color)
{
	if(screen == GFX_ALL) {
		for(int i = 0; i < GFX_ALL; i++)
			gfx_clear(i, color);
	} else {
	    for(int i = 0; i < fbs[screen].width * fbs[screen].height; i++)
	        fbs[screen].ptr[i] = color;

	    gfx_draw_logo(screen);

	    fbs[screen].current_x = 10;
	    fbs[screen].current_y = 10;
	}
}

#define LOGO_TV_SCALE 13
#define LOGO_DRC_SCALE 8
#define LOGO_X(gfx, scale) ((fbs[gfx].width - (LOGO_W * scale)) / 2)
#define LOGO_Y(gfx, scale) ((fbs[gfx].height - (LOGO_H * scale)) / 2)

void SRAM_TEXT gfx_draw_logo(gfx_screen_t screen) {
	if (screen == GFX_TV) {
		for (int y = 0; y < LOGO_H; y++) {
			for (int x = 0; x < LOGO_W; x++) {
				for (int y2 = 0; y2 < LOGO_TV_SCALE; y2++) {
					for (int x2 = 0; x2 < LOGO_TV_SCALE; x2++) {
						int x3 = (x * LOGO_TV_SCALE) + x2 + LOGO_X(GFX_TV, LOGO_TV_SCALE);
						int y3 = (y * LOGO_TV_SCALE) + y2 + LOGO_Y(GFX_TV, LOGO_TV_SCALE);
						fbs[screen].ptr[x3 + y3*fbs[screen].width] = ((uint32_t*)lw_logo)[x + y*LOGO_W];
					}
				}
			}
		}
	} else if (screen == GFX_DRC) {
		for (int y = 0; y < LOGO_H; y++) {
			for (int x = 0; x < LOGO_W; x++) {
				for (int y2 = 0; y2 < LOGO_DRC_SCALE; y2++) {
					for (int x2 = 0; x2 < LOGO_DRC_SCALE; x2++) {
						int x3 = (x * LOGO_DRC_SCALE) + x2 + LOGO_X(GFX_DRC, LOGO_DRC_SCALE);
						int y3 = (y * LOGO_DRC_SCALE) + y2 + LOGO_Y(GFX_DRC, LOGO_DRC_SCALE);
						fbs[screen].ptr[x3 + y3*fbs[screen].width] = ((uint32_t*)lw_logo)[x + y*LOGO_W];
					}
				}
			}
		}
	}
}

void SRAM_TEXT gfx_draw_char(gfx_screen_t screen, char c, int x, int y, u32 color)
{
	if(screen == GFX_ALL) {
		for(int i = 0; i < GFX_ALL; i++)
			gfx_draw_char(i, c, x, y, color);
	} else {
		if(c < 32) return;
		c -= 32;

		const u8* charData = &msx_font[(CHAR_SIZE_X * CHAR_SIZE_Y * c) / 8];
		u32* fb = &fbs[screen].ptr[x + y * (gfx_get_stride(screen) / sizeof(u32))];

		for(int i = 0; i < CHAR_SIZE_Y; ++i)
		{
			u8 v = *(charData++);

			for(int j = 0; j < CHAR_SIZE_X; ++j)
			{
				if(v & (128 >> j)) *fb = color;
				else *fb = 0x00000000;
				fb++;
			}

			fb += (gfx_get_stride(screen) / sizeof(u32)) - CHAR_SIZE_X;
		}
	}
}

void SRAM_TEXT gfx_draw_string(gfx_screen_t screen, const char* str, int x, int y, u32 color)
{
	if(screen == GFX_ALL) {
		for(int i = 0; i < GFX_ALL; i++)
			gfx_draw_string(i, str, x, y, color);
	} else {
		if(!str) return;

		int dx = 0, dy = 0;
		for(int k = 0; str[k]; k++)
		{
			if(str[k] >= 32 && str[k] < 128)
				gfx_draw_char(screen, str[k], x + dx, y + dy, color);

			dx += 8;

			if(str[k] == '\n')
			{
				dx = 0;
				dy -= 8;
			}
		}
	}
}

void SRAM_TEXT sram_print(const char* msg) {
	int lines = 0;
	int width = fbs[GFX_DRC].current_x;
	for (int k = 0; msg[k]; k++) {
		width += CHAR_SIZE_X;
		if (msg[k] == '\n') {
			lines += 10;
			width = 0;
		}
	}
	if (fbs[GFX_DRC].current_y + lines >= fbs[GFX_DRC].height - 20) {
		gfx_clear(GFX_DRC, BLACK);
	}
	gfx_draw_string(GFX_DRC, msg, fbs[GFX_DRC].current_x, fbs[GFX_DRC].current_y, GREEN);

	fbs[GFX_DRC].current_y += lines;
	fbs[GFX_DRC].current_x = width;
}

void SRAM_TEXT sram_print_hex(u32 hex) {
	char msg[11];
	msg[0] = '0'; msg[1] = 'x';

	for (int i = 0; i < 8 ; i++) {
		char digit = (hex >> ((7-i)*4)) & 0xf;
		digit += (digit < 0xA) ? '0' : ('a'-0xA);

		msg[i+2] = digit;
	}

	msg[10] = '\0';

	sram_print(msg);
}

// This sucks, should use a stdout devoptab.
int printf(const char* fmt, ...)
{
	static char str[0x800];
	va_list va;

	va_start(va, fmt);
	vsnprintf(str, sizeof(str), fmt, va);
	va_end(va);

	int lines = 0;
	//char* last_line = str;
	for(int k = 0; str[k]; k++)
	{
		if(str[k] == '\n')
		{
			lines += 10;
			//last_line = &str[k + 1];
		}
	}

	//for(int i = 0; i < GFX_ALL; i++) {
		if(fbs[GFX_DRC].current_y + lines >= fbs[GFX_DRC].height - 20)
			gfx_clear(GFX_DRC, BLACK);

		gfx_draw_string(GFX_DRC, str, /* current_x */ 10, fbs[GFX_DRC].current_y, WHITE);
		//current_x += strlen(last_line);
		fbs[GFX_DRC].current_y += lines;
	//}

	return 0;
}
