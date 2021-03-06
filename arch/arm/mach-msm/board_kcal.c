/*
 * arch/arm/mach-msm/board_kcal.c
 *
 * Copyright (C) 2014 Savoca
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/persistent_ram.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <asm/setup.h>
#include <asm/system_info.h>
#include <mach/kcal.h>

int g_kcal_r = 255;
int g_kcal_g = 255;
int g_kcal_b = 255;

extern int kcal_set_values(int kcal_r, int kcal_g, int kcal_b);
static int __init display_kcal_setup(char *kcal)
{
	char vaild_k = 0;
	int kcal_r = 255;
	int kcal_g = 255;
	int kcal_b = 255;

	sscanf(kcal, "%d|%d|%d|%c", &kcal_r, &kcal_g, &kcal_b, &vaild_k );
	pr_info("kcal: %d|%d|%d|%c\n", kcal_r, kcal_g, kcal_b, vaild_k);

	if (vaild_k != 'K') {
		pr_info("kcal: Not calibrated yet : %d\n", vaild_k);
		kcal_r = kcal_g = kcal_b = 255;
	}

	kcal_set_values(kcal_r, kcal_g, kcal_b);
	return 1;
}
__setup("lcd.kcal=", display_kcal_setup);
