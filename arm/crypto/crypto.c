/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  Copyright (C) 2008, 2009    Haxx Enterprises <bushing@gmail.com>
 *  Copyright (C) 2008, 2009    Sven Peter <svenpeter@gmail.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "crypto.h"
#include "system/latte.h"
#include "common/utils.h"
#include "system/memory.h"
#include "system/irq.h"
#include "video/gfx.h"
#include "string.h"
#include "seeprom.h"
#include "aes.h"

otp_t otp = {
        .otpstore_magic = {0x4F545053, 0x544F5245, 0x4F545053, 0x544F5245},
};
seeprom_t seeprom;

void crypto_read_otp(void) {
    printf("crypto: %08x %08x %08x %08x\n", otp.otpstore_magic[0], otp.otpstore_magic[1], otp.otpstore_magic[2], otp.otpstore_magic[3]);
    if (otp.otpstore_magic[0] != 0x4F545053 ||
        otp.otpstore_magic[1] != 0x544F5245 ||
        otp.otpstore_magic[2] != 0x4F545053 ||
        otp.otpstore_magic[3] != 0x544F5245) {

        printf("crypto: using de_Fuse otp store\n");
        return;
    }

    u8 *otpd = (u8 *) &otp;
    int word, bank;
    for (bank = 0; bank < 8; bank++) {
        for (word = 0; word < 0x20; word++) {
            write32(LT_OTPCMD, 0x80000000 | bank << 8 | word);
            u32 data = read32(LT_OTPDATA);
            *otpd++ = (data >> 24) & 0xff;
            *otpd++ = (data >> 16) & 0xff;
            *otpd++ = (data >> 8) & 0xff;
            *otpd++ = (data >> 0) & 0xff;
        }
    }
}

void crypto_read_seeprom(void) {
    seeprom_read(&seeprom, 0, sizeof(seeprom) / 2);
}

void crypto_initialize(void) {
    crypto_read_otp();
    crypto_read_seeprom();
}
