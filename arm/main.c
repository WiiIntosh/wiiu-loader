/*
 *	It's a Project! linux-loader
 *
 *	Copyright (C) 2017          Ash Logan <quarktheawesome@gmail.com>
 *
 *	Based on code from the following contributors:
 *
 *	Copyright (C) 2016          SALT
 *	Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *	Copyright (C) 2008, 2009    Haxx Enterprises <bushing@gmail.com>
 *	Copyright (C) 2008, 2009    Sven Peter <svenpeter@gmail.com>
 *	Copyright (C) 2008, 2009    Hector Martin "marcan" <marcan@marcansoft.com>
 *	Copyright (C) 2009          Andre Heider "dhewg" <dhewg@wiibrew.org>
 *	Copyright (C) 2009          John Kelley <wiidev@kelley.ca>
 *
 *	(see https://github.com/Dazzozo/minute)
 *
 *	This code is licensed to you under the terms of the GNU GPL, version 2;
 *	see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include <stdlib.h>
#include "video/gfx.h"
#include "system/abif.h"
#include "system/ppc.h"
#include "system/exception.h"
#include "system/memory.h"
#include "system/irq.h"
#include "storage/sd/sdcard.h"
#include "storage/sd/fatfs/elm.h"
#include "storage/isfs.h"
#include "storage/crypto.h"
#include "application.h"
#include "system/smc.h"
#include "system/latte.h"
#include "common/utils.h"

void NORETURN _main(void* base) {
	//Set up framebuffer/logging
	abif_gpu_setup();
	gfx_clear(GFX_ALL, BLACK);
	printf("Hello World!\n");

	//Initialize everything
	exception_initialize();
	printf("[ OK ] Setup Exceptions\n");
	mem_initialize();
	printf("[ OK ] Turned on Caches/MMU\n");

	irq_initialize();
	printf("[ OK ] Setup Interrupts\n");

	srand(read32(LT_TIMER));
	crypto_initialize();
	printf("[ OK ] Setup Crypto\n");

	sdcard_init();
	printf("[ OK ] Setup SD Card\n");

	int res = ELM_Mount();
	if (res) {
		char errorstr[] = "Couldn't mount SD card! See Gamepad for details.";
		gfx_draw_string(GFX_TV, errorstr, (1280 - sizeof(errorstr)*8) / 2, 500, WHITE);
		printf("[FATL] SD Card mount error: %d\n", res);
		panic(0);
	}
	printf("[ OK ] Mounted SD Card\n");

	isfs_init();
	printf("[ OK ] Mounted SLC\n");

	write32(LT_AHBPROT, 0xFFFFFFFF);
	write32(LT_GPIO_ENABLE, 0xFFFFFFFF);
	write32(LT_GPIO_OWNER, 0xFFFFFFFF);
	printf("[ OK ] Unrestricted Hardware\n");

	printf("--------------------------\n");
	printf("          Ready!          \n");
	printf("--------------------------\n");
	//We're good to go!

	app_run();
}
