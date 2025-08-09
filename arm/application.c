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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common/utils.h"
#include "storage/nand/isfs/isfs.h"
#include "storage/nand/nand.h"
#include "storage/sd/fatfs/elm.h"
#include "storage/sd/sdcard.h"
#include "system/irq.h"
#include "system/latte.h"
#include "system/memory.h"
#include "system/ppc_elf.h"
#include "system/smc.h"
#include "video/gfx.h"

#include "application.h"
#include "ipc_protocol.h"

#define PPC_BOOT_FILE "/openbios.elf"

#define LT_IPC_ARMCTRL_COMPAT_X1 0x4
#define LT_IPC_ARMCTRL_COMPAT_Y1 0x1

u32 SRAM_DATA ppc_entry = 0;
u32 SRAM_DATA rtc_bias = 0;

static u32 read_rtc_bias() {
	//
	// Read the RTC counter bias from rtc.xml on slc. Credit: Rairii
	//
    u32 RtcBias = 0;
    FILE* fRtc = fopen("slc:/sys/config/rtc.xml", "rb");
    if (fRtc != NULL) {
        char data[2048] = {0};
        fread(data, 1, sizeof (data), fRtc);
        fclose(fRtc);
        char *rtcEnd = strstr(data, "</rtc_offset>");
        if (rtcEnd != NULL) {
            char *ptr = rtcEnd - 1;
            char *rtcStart = rtcEnd - 1;
            while (rtcStart[-1] != '>') {
				rtcStart--;
			}
            *rtcEnd = 0;
            u32 base = 1;
            while (ptr >= rtcStart) {
                char chr = ptr[0];
                if (chr < '0' || chr > '9') {
					break;
				}
                RtcBias += (base * (chr - '0'));
                base *= 10;
                ptr--;
            }
        }
    }

    if (RtcBias == 0) {
		printf("Failed to read or parse RTC bias from SLC\n");
	}
	return RtcBias;
} 

static void SRAM_TEXT NORETURN app_run_sram() {
	// Uncomment to completely erase linux-loader from MEM2
	// memset32((void*)0x10000000, 0, 0x800000);
	gfx_clear(GFX_DRC, BLACK);
	// Start the PowerPC
	write32(0x14000000, ppc_entry);
	for (;;) {
		// check for message sent flag
		u32 ctrl = read32(LT_IPC_ARMCTRL_COMPAT);
		if (!(ctrl & LT_IPC_ARMCTRL_COMPAT_X1))
			continue;

		// read PowerPC's message
		u32 msg = read32(LT_IPC_PPCMSG_COMPAT);

		// process commands
		if (msg == CMD_POWEROFF) {
			smc_shutdown(false);
		} else if (msg == CMD_REBOOT) {
			smc_shutdown(true);
		} else if (msg == CMD_RTC_BIAS) {
			write32(LT_IPC_ARMMSG_COMPAT, rtc_bias);
		} else if ((msg & CMD_PRINT_MASK) == CMD_PRINT) {
			char str[2];
			str[0] = msg & 0xFF;
			str[1] = '\0';
			sram_print(str);
		}

		// writeback ctrl value to reset IPC
		write32(LT_IPC_ARMCTRL_COMPAT, ctrl);
	}
}

void NORETURN app_run() {
	int res;

	// Load up OpenBIOS to run on the PowerPC.
	res = ppc_load_file(PPC_BOOT_FILE, &ppc_entry);
	if (res < 0) {
		char errorstr[] = "Loading OpenBIOS failed! See Gamepad for details.";
		gfx_draw_string(GFX_TV, errorstr, (1280 - sizeof(errorstr) * 8) / 2, 500, WHITE);
		printf("[FATL] Loading OpenBIOS failed! (%d)\n", res);
		panic(0);
	}
	printf("[ OK ] Loaded OpenBIOS (%d). Entry is %08lX.\n", res, ppc_entry);

	// Read RTC bias for IPC use.
	rtc_bias = read_rtc_bias();

	// Shut everything down, ready for SRAM switch
	ELM_Unmount();
	sdcard_exit();
	irq_disable(IRQ_SD0);
	isfs_fini();
	nand_deinitialize();
	irq_shutdown();
	printf("[ OK ] Unmounted filesystems and removed interrupts.\n");
	mem_shutdown();
	printf("[ OK ] Disabled caches/MMU\n");

	printf("[BYE ] Doing SRAM context switch...\n");

	// Move execution to SRAM. OpenBIOS will overwrite everything in MEM2, including this code.
	sram_ctx_switch(&app_run_sram);
}
