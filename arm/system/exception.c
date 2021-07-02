/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2008, 2009    Hector Martin "marcan" <marcan@marcansoft.com>
 *  Copyright (C) 2008, 2009    Haxx Enterprises <bushing@gmail.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "system/irq.h"
#include "video/gfx.h"
#include "common/utils.h"
#include "system/memory.h"

//it's infuriating that gcc seems to require this
static const char sReset[] SRAM_DATA2 = "RESET";
static const char sUndIns[] SRAM_DATA2 = "UNDEFINED INSTR";
static const char sSWI[] SRAM_DATA2 = "SWI";
static const char sInstr[] SRAM_DATA2 = "INSTR ABORT";
static const char sData[] SRAM_DATA2 = "DATA ABORT";
static const char sRsvd[] SRAM_DATA2 = "RESERVED";
static const char sIRQ[] SRAM_DATA2 = "IRQ";
static const char sFIQ[] SRAM_DATA2 = "FIQ";
static const char sUnk[] SRAM_DATA2 = "(unknown exception type)";

static const char * const exceptions[] SRAM_DATA2 = {
    sReset, sUndIns, sSWI, sInstr, sData, sRsvd, sIRQ, sFIQ, sUnk
};

static const char sUND[] SRAM_DATA2 = "UNDEFINED";
static const char sAlign[] SRAM_DATA2 = "Alignment";
static const char sTrans[] SRAM_DATA2 = "Translation";
static const char sExtAbrt[] SRAM_DATA2 = "External abort";
static const char sDomain[] SRAM_DATA2 = "Domain";
static const char sExtTrans1st[] SRAM_DATA2 = "External abort on translation (first level)";
static const char sPerms[] SRAM_DATA2 = "Permission";
static const char sExtTrans2nd[] SRAM_DATA2 = "External abort on translation (second level)";

static const char * const aborts[] SRAM_DATA2 = {
    sUND,
    sAlign,
    sUND,
    sAlign,
    sUND,
    sTrans,
    sUND,
    sTrans,
    sExtAbrt,
    sDomain,
    sExtAbrt,
    sDomain,
    sExtTrans1st,
    sPerms,
    sExtTrans2nd,
    sPerms,
};

static const u8 SRAM_DATA domvalid[] = {0,0,0,0,0,0,0,1,0,1,0,1,0,1,1,1};

void exc_setup_stack(void);

void exception_initialize(void)
{
    exc_setup_stack();
    u32 cr = get_cr();
    cr |= 0x2; // Data alignment fault checking enable
    set_cr(cr);
}

void SRAM_TEXT exc_handler(u32 type, u32 spsr, u32 *regs)
{
    (void) spsr;

    gfx_clear(GFX_ALL, BLACK);

    if (type > 8) type = 8;
    //this is the best I could do to give formatted printing with near-zero
    //binary size/stack use
    spr("Exception ");sph(type);spch(' ');spc(exceptions[type]);spch('\n');

    u32 pc, fsr;

    switch(type) {
        case 1: // UND
        case 2: // SWI
        case 3: // INSTR ABORT
        case 7: // FIQ
            pc = regs[15] - 4;
            break;
        case 4: // DATA ABORT
            pc = regs[15] - 8;
            break;
        default:
            pc = regs[15];
            break;
    }

    spr("Registers: ");sph(regs);spch('\n');
    spr("  R0-R3: ");sphs(regs[0]);sphs(regs[1]);sphs(regs[2]);sph(regs[3]);spch('\n');
    spr("  R4-R7: ");sphs(regs[4]);sphs(regs[5]);sphs(regs[6]);sph(regs[7]);spch('\n');
    spr(" R8-R11: ");sphs(regs[8]);sphs(regs[9]);sphs(regs[10]);sph(regs[11]);spch('\n');
    spr("R12-R15: ");sphs(regs[12]);sphs(regs[13]);sphs(regs[14]);sph(regs[15]);spch('\n');

    spr("SPSR: ");sph(spsr);spch('\n');
    spr("CPSR: ");sph(get_cpsr());spch('\n');
    spr("CR:   ");sph(get_cr());spch('\n');
    spr("TTBR: ");sph(get_ttbr());spch('\n');
    spr("DACR: ");sph(get_dacr());spch('\n');

    switch (type) {
        case 3: // INSTR ABORT
        case 4: // DATA ABORT
            if(type == 3)
                fsr = get_ifsr();
            else
                fsr = get_dfsr();
            spr("Abort type: ");spc(aborts[fsr&0xf]);spch('\n');
            if(domvalid[fsr&0xf]) {
                spr("Domain: ");sph((fsr>>4)&0xf);spch('\n');
            }if(type == 4) {
                spr("Address: ");sph(get_far());spch('\n');
            }
        break;
        default: break;
    }

    if(type != 3) {
        spr("Code dump:\n");
        sph(pc-16);spch(':');sphs(read32(pc-16));sphs(read32(pc-12));sphs(read32(pc-8));sph(read32(pc-4));spch('\n');
        sph(pc);spch('*');sphs(read32(pc));sphs(read32(pc+4));sphs(read32(pc+8));sph(read32(pc+12));spch('\n');
        sph(pc+16);spch(':');sphs(read32(pc+16));sphs(read32(pc+20));sphs(read32(pc+24));sph(read32(pc+28));spch('\n');
    }

	panic(0);
}
