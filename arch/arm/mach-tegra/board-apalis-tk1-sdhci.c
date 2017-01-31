/*
 * arch/arm/mach-tegra/board-apalis-tk1-sdhci.c
 *
 * Copyright (c) 2016, Toradex AG.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/wlan_plat.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/mmc/host.h>
#include <linux/wl12xx.h>
#include <linux/platform_data/mmc-sdhci-tegra.h>
#include <linux/mfd/max77660/max77660-core.h>
#include <linux/tegra-fuse.h>
#include <linux/dma-mapping.h>

#include <asm/mach-types.h>
#include <mach/irqs.h>
#include <mach/gpio-tegra.h>
#include <mach/nct.h>

#include "gpio-names.h"
#include "board.h"
#include "board-apalis-tk1.h"
#include "dvfs.h"
#include "iomap.h"
#include "tegra-board-id.h"

#define FUSE_SOC_SPEEDO_0	0x134

static struct resource sdhci_resource0[] = {
	[0] = {
		.start	= INT_SDMMC1,
		.end	= INT_SDMMC1,
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC1_BASE,
		.end	= TEGRA_SDMMC1_BASE + TEGRA_SDMMC1_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource sdhci_resource2[] = {
	[0] = {
		.start	= INT_SDMMC3,
		.end	= INT_SDMMC3,
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC3_BASE,
		.end	= TEGRA_SDMMC3_BASE + TEGRA_SDMMC3_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource sdhci_resource3[] = {
	[0] = {
		.start	= INT_SDMMC4,
		.end	= INT_SDMMC4,
		.flags	= IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC4_BASE,
		.end	= TEGRA_SDMMC4_BASE + TEGRA_SDMMC4_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

#ifdef CONFIG_MMC_EMBEDDED_SDIO
static struct embedded_sdio_data embedded_sdio_data0 = {
	.cccr = {
		.sdio_vsn	= 2,
		.multi_block	= 1,
		.low_speed	= 0,
		.wide_bus	= 0,
		.high_power	= 1,
		.high_speed	= 1,
	},
	.cis = {
		.vendor	= 0x02d0,
		.device	= 0x4329,
	},
};
#endif

static u64 tegra_sdhci_dmamask = DMA_BIT_MASK(64);

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data0 = {
	.cd_gpio		= TEGRA_GPIO_PV3,
	.wp_gpio		= -1,
	.power_gpio		= -1,
	.tap_delay		= 0,
	.trim_delay		= 0x2,
/*	.trim_delay		= 0x3, */
/* TBD
	.ddr_clk_limit		= 41000000, */
	.uhs_mask		= MMC_UHS_MASK_DDR50,
	.calib_3v3_offsets	= 0x7676,
	.calib_1v8_offsets	= 0x7676,
/* TBD	.max_clk_limit		= 136000000, */
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data2 = {
#ifdef APALIS_TK1_V10
	.cd_gpio		= -1,
#else
	.cd_gpio		= TEGRA_GPIO_PV2,
#endif
	.wp_gpio		= -1,
	.power_gpio		= -1,
	.tap_delay		= 0,
/*	.trim_delay		= 0x2, */
	.trim_delay		= 0x3,
	.uhs_mask		= MMC_UHS_MASK_DDR50,
	.calib_3v3_offsets	= 0x7676,
	.calib_1v8_offsets	= 0x7676,
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data3 = {
	.cd_gpio		= -1,
	.wp_gpio		= -1,
	.power_gpio		= -1,
	.is_8bit		= 1,
	.tap_delay		= 0x4,
	.trim_delay		= 0x4,
	.ddr_trim_delay		= 0x0,
	.mmc_data = {
		.built_in	= 1,
		.ocr_mask	= MMC_OCR_1V8_MASK,
	},
	.ddr_clk_limit		= 51000000,
	.max_clk_limit		= 200000000,
	.calib_3v3_offsets	= 0x0202,
	.calib_1v8_offsets	= 0x0202,
};

static struct platform_device tegra_sdhci_device0 = {
	.name		= "sdhci-tegra",
	.id		= 0,
	.resource	= sdhci_resource0,
	.num_resources	= ARRAY_SIZE(sdhci_resource0),
	.dev = {
		.dma_mask		= &tegra_sdhci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &tegra_sdhci_platform_data0,
	},
};

static struct platform_device tegra_sdhci_device2 = {
	.name		= "sdhci-tegra",
	.id		= 2,
	.resource	= sdhci_resource2,
	.num_resources	= ARRAY_SIZE(sdhci_resource2),
	.dev = {
		.dma_mask		= &tegra_sdhci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &tegra_sdhci_platform_data2,
	},
};

static struct platform_device tegra_sdhci_device3 = {
	.name		= "sdhci-tegra",
	.id		= 3,
	.resource	= sdhci_resource3,
	.num_resources	= ARRAY_SIZE(sdhci_resource3),
	.dev = {
		.dma_mask		= &tegra_sdhci_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(64),
		.platform_data		= &tegra_sdhci_platform_data3,
	},
};

int __init apalis_tk1_sdhci_init(void)
{
	int nominal_core_mv;
	int min_vcore_override_mv;
	int boot_vcore_mv;
	u32 speedo;

	nominal_core_mv =
			tegra_dvfs_rail_get_nominal_millivolts(tegra_core_rail);
	if (nominal_core_mv) {
		tegra_sdhci_platform_data0.nominal_vcore_mv = nominal_core_mv;
		tegra_sdhci_platform_data2.nominal_vcore_mv = nominal_core_mv;
		tegra_sdhci_platform_data3.nominal_vcore_mv = nominal_core_mv;
	}
	min_vcore_override_mv =
			tegra_dvfs_rail_get_override_floor(tegra_core_rail);
	if (min_vcore_override_mv) {
		tegra_sdhci_platform_data0.min_vcore_override_mv =
				min_vcore_override_mv;
		tegra_sdhci_platform_data2.min_vcore_override_mv =
				min_vcore_override_mv;
		tegra_sdhci_platform_data3.min_vcore_override_mv =
				min_vcore_override_mv;
	}
	boot_vcore_mv = tegra_dvfs_rail_get_boot_level(tegra_core_rail);
	if (boot_vcore_mv) {
		tegra_sdhci_platform_data0.boot_vcore_mv = boot_vcore_mv;
		tegra_sdhci_platform_data2.boot_vcore_mv = boot_vcore_mv;
		tegra_sdhci_platform_data3.boot_vcore_mv = boot_vcore_mv;
	}

/* TBD
	tegra_sdhci_platform_data2.max_clk_limit = 204000000;

	tegra_sdhci_platform_data0.default_drv_type =
			MMC_SET_DRIVER_TYPE_A;

	tegra_sdhci_platform_data0.max_clk_limit = 204000000;

	tegra_sdhci_platform_data3.uhs_mask = MMC_MASK_HS200;
TBD */

	/*
	 * FIXME: Set max clk limit to 200MHz for SDMMC3 for PM375.
	 * Requesting 208MHz results in getting 204MHz from PLL_P
	 * and CRC errors are seen with same.
	 */
	tegra_sdhci_platform_data0.max_clk_limit = 200000000;
	tegra_sdhci_platform_data2.max_clk_limit = 200000000;

	speedo = tegra_fuse_readl(FUSE_SOC_SPEEDO_0);
	tegra_sdhci_platform_data0.cpu_speedo = speedo;
	tegra_sdhci_platform_data2.cpu_speedo = speedo;
	tegra_sdhci_platform_data3.cpu_speedo = speedo;

    tegra_sdhci_platform_data0.disable_clock_gate = 1;
    tegra_sdhci_platform_data2.disable_clock_gate = 1;

/* TBD
	tegra_sdhci_platform_data0.uhs_mask =
			MMC_UHS_MASK_SDR50 | MMC_UHS_MASK_DDR50;
	tegra_sdhci_platform_data2.uhs_mask =
			MMC_UHS_MASK_SDR50; */

/* TBD
	tegra_sdhci_platform_data0.max_clk_limit = 204000000; */

/* TBD
	tegra_sdhci_platform_data2.uhs_mask =
			MMC_UHS_MASK_SDR50;
	tegra_sdhci_platform_data0.uhs_mask =
			MMC_UHS_MASK_SDR50;
	tegra_sdhci_platform_data3.max_clk_limit = 200000000;
	tegra_sdhci_platform_data2.max_clk_limit = 204000000; */

	platform_device_register(&tegra_sdhci_device3);
	platform_device_register(&tegra_sdhci_device0);
	platform_device_register(&tegra_sdhci_device2);

	return 0;
}
