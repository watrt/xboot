/*
 * arch/arm/mach-smdk6410/mach-smdk6410.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <macros.h>
#include <io.h>
#include <mode/mode.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <s3c6410-cp15.h>
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-wdg.h>


extern x_u8	__text_start[];
extern x_u8 __text_end[];
extern x_u8 __ramdisk_start[];
extern x_u8 __ramdisk_end[];
extern x_u8 __data_shadow_start[];
extern x_u8 __data_shadow_end[];
extern x_u8 __data_start[];
extern x_u8 __data_end[];
extern x_u8 __bss_start[];
extern x_u8 __bss_end[];
extern x_u8 __heap_start[];
extern x_u8 __heap_end[];
extern x_u8 __stack_start[];
extern x_u8 __stack_end[];

/*
 * system initial, like power lock
 */
static void mach_init(void)
{
	/* GPK13 and GPK15 power lock */
	writel(S3C6410_GPKCON1, (readl(S3C6410_GPKCON1) & ~(0xf<<20)) | (0x1<<20));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<26)) | (0x2<<26));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<13)) | (0x1<<13));
	writel(S3C6410_GPKCON1, (readl(S3C6410_GPKCON1) & ~(0xf<<28)) | (0x1<<28));
	writel(S3C6410_GPKPUD, (readl(S3C6410_GPKPUD) & ~(0x3<<30)) | (0x2<<30));
	writel(S3C6410_GPKDAT, (readl(S3C6410_GPKDAT) & ~(0x1<<15)) | (0x1<<15));
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static x_bool mach_reset(void)
{
	/* disable watchdog */
	writel(S3C6410_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S3C6410_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S3C6410_WTCON, 0x0021);

	return TRUE;
}

/*
 * get system mode
 */
static enum mode mach_getmode(void)
{
	return MODE_MENU;
}

/*
 * clean up system before running os.
 */
x_bool mach_cleanup(void)
{
	/* disable irq */
	irq_disable();

	/* disable fiq */
	fiq_disable();

	/* disable icache */
	icache_disable();

	/* disable dcache */
	dcache_disable();

	/* disable mmu */
	mmu_disable();

	/* disable vic */
	vic_disable();

	return TRUE;
}

/*
 * for anti-piracy
 */
static x_bool mach_genuine(void)
{
	return TRUE;
}

/*
 * a portable data interface for machine.
 */
static struct machine smdk6410 = {
	.info = {
		.board_name 		= "smdk6410",
		.board_desc 		= "smdk6410 board by samsung",
		.board_id			= "0",

		.cpu_name			= "s3c6410x",
		.cpu_desc			= "based on arm11 by samsung",
		.cpu_id				= "0x410fb760",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x50000000,
				.end		= 0x50000000 + SZ_128M - 1,
			},

			[1] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 12*1000*1000,
	},

	.link = {
		.text_start			= (const x_sys)__text_start,
		.text_end			= (const x_sys)__text_end,

		.ramdisk_start		= (const x_sys)__ramdisk_start,
		.ramdisk_end		= (const x_sys)__ramdisk_end,

		.data_shadow_start	= (const x_sys)__data_shadow_start,
		.data_shadow_end	= (const x_sys)__data_shadow_end,

		.data_start			= (const x_sys)__data_start,
		.data_end			= (const x_sys)__data_end,

		.bss_start			= (const x_sys)__bss_start,
		.bss_end			= (const x_sys)__bss_end,

		.heap_start			= (const x_sys)__heap_start,
		.heap_end			= (const x_sys)__heap_end,

		.stack_start		= (const x_sys)__stack_start,
		.stack_end			= (const x_sys)__stack_end,
	},

	.pm = {
		.init 				= mach_init,
		.suspend			= NULL,
		.resume				= NULL,
		.halt				= NULL,
		.reset				= mach_reset,
	},

	.misc = {
		.getmode			= mach_getmode,
		.cleanup			= mach_cleanup,
		.genuine			= mach_genuine,
	},

	.priv					= NULL,
};

static __init void mach_smdk6410_init(void)
{
	if(!machine_register(&smdk6410))
		LOG_E("failed to register machine 'smdk6410'");
}

module_init(mach_smdk6410_init, LEVEL_MACH);
