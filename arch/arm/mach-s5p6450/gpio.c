/* arch/arm/mach-s5p6450/gpio.c
 *
 * Copyright (c) 2009 Samsung Electronics Co., Ltd.
 * 		http://www.samsung.com/
 *
 * S5P6450 - GPIOlib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <mach/map.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-core.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg-helpers.h>

/* GPIO bank summary:
*
* Bank	GPIOs	Style	SlpCon	ExtInt Group
* A		6		4Bit	Yes			1
* B		7		4Bit	Yes			1
* C		8		4Bit	Yes			2
* D		8		4Bit	Yes			None
* F		2		2Bit	Yes			None
* G		14		4Bit[2]	Yes			5
* H		10		4Bit[2]	Yes			6
* I		16		2Bit	Yes			None
* J		12		2Bit	Yes			None
* K		5		4Bit	Yes			None
* N		16		2Bit	No			Ext_Int
* P		11		2Bit	Yes			8
* Q		14		2Bit	Yes			None
* R		15		4Bit[2]	Yes			8
* S		8		2Bit	Yes			None
*
* [1] BANKF pins 14,15 do not form part of the external interrupt sources
* [2] BANK has two control registers, GPxCON0 and GPxCON1
*/

static int s5p6450_gpiolib_rbank_4bit2_input(struct gpio_chip *chip,
					     unsigned int offset)
{
	struct s3c_gpio_chip *ourchip = to_s3c_gpio(chip);
	void __iomem *base = ourchip->base;
	void __iomem *regcon = base;
	unsigned long con;

	switch (offset) {
	case 6:
		offset += 1;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		regcon -= 4;
		break;
	default:
		offset -= 7;
		break;
	}

	con = __raw_readl(regcon);
	con &= ~(0xf << con_4bit_shift(offset));
	__raw_writel(con, regcon);

	return 0;
}

static int s5p6450_gpiolib_rbank_4bit2_output(struct gpio_chip *chip,
					      unsigned int offset, int value)
{
	struct s3c_gpio_chip *ourchip = to_s3c_gpio(chip);
	void __iomem *base = ourchip->base;
	void __iomem *regcon = base;
	unsigned long con;
	unsigned long dat;
	unsigned con_offset  = offset;

	switch (con_offset) {
	case 6:
		con_offset += 1;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		regcon -= 4;
		break;
	default:
		con_offset -= 7;
		break;
	}

	con = __raw_readl(regcon);
	con &= ~(0xf << con_4bit_shift(con_offset));
	con |= 0x1 << con_4bit_shift(con_offset);

	dat = __raw_readl(base + GPIODAT_OFF);
	if (value)
		dat |= 1 << offset;
	else
		dat &= ~(1 << offset);

	__raw_writel(con, regcon);
	__raw_writel(dat, base + GPIODAT_OFF);

	return 0;
}

int s5p6450_gpio_setcfg_4bit_rbank(struct s3c_gpio_chip *chip,
				   unsigned int off, unsigned int cfg)
{
	void __iomem *reg = chip->base;
	unsigned int shift;
	u32 con;

	switch (off) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		shift = (off & 7) * 4;
		reg -= 4;
		break;
	case 6:
		shift = ((off + 1) & 7) * 4;
		reg -= 4;
	default:
		shift = ((off + 1) & 7) * 4;
		break;
	}

	if (s3c_gpio_is_cfg_special(cfg)) {
		cfg &= 0xf;
		cfg <<= shift;
	}

	con = __raw_readl(reg);
	con &= ~(0xf << shift);
	con |= cfg;
	__raw_writel(con, reg);

	return 0;
}

static struct s3c_gpio_cfg s5p6450_gpio_cfgs[] = {
	{
		.cfg_eint	= 0,
	}, {
		.cfg_eint	= 7,
	}, {
		.cfg_eint	= 3,
		.set_config	= s5p6450_gpio_setcfg_4bit_rbank,
	}, {
		.cfg_eint	= 0,
		.set_config	= s3c_gpio_setcfg_s3c24xx,
	}, {
		.cfg_eint	= 2,
		.set_config	= s3c_gpio_setcfg_s3c24xx,
	}, {
		.cfg_eint	= 3,
		.set_config	= s3c_gpio_setcfg_s3c24xx,
	},
};
static int s3c_gpa_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(1, offset);
}

static int s3c_gpb_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(1, offset + 8);
}

static int s3c_gpc_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(2, offset);
}

static struct s3c_gpio_chip s5p6450_gpio_4bit[] = {
	{
		.base	= S5P6450_GPA_BASE,
		.config	= &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPA(0),
			.ngpio	= S5P6450_GPIO_A_NR,
			.to_irq	= s3c_gpa_to_irq,
			.label	= "GPA",
		},
	}, {
		.base	= S5P6450_GPB_BASE,
		.config	= &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPB(0),
			.ngpio	= S5P6450_GPIO_B_NR,
			.to_irq	= s3c_gpb_to_irq,
			.label	= "GPB",
		},
	}, {
		.base	= S5P6450_GPC_BASE,
		.config	= &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPC(0),
			.ngpio	= S5P6450_GPIO_C_NR,
			.to_irq	= s3c_gpc_to_irq,
			.label	= "GPC",
		},
	}, {
		.base	= S5P6450_GPD_BASE,
		.config	= &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPD(0),
			.ngpio	= S5P6450_GPIO_D_NR,
			.label	= "GPD",
			},
		}, {
		.base	= S5P6450_GPK_BASE,
		.config = &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPK(0),
			.ngpio	= S5P6450_GPIO_K_NR,
			.label	= "GPK",
		},
	},
};

static int s3c_gpg_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(5, offset);
}

static int s3c_gph_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(6, offset);
}

static struct s3c_gpio_chip s5p6450_gpio_4bit2[] = {
	{
		.base	= S5P6450_GPG_BASE + 0x4,
		.config	= &s5p6450_gpio_cfgs[1],
		.chip	= {
			.base	= S5P6450_GPG(0),
			.ngpio	= S5P6450_GPIO_G_NR,
			.to_irq	= s3c_gpg_to_irq,
			.label	= "GPG",
		},
	}, {
		.base   = S5P6450_GPH_BASE + 0x4,
		.config = &s5p6450_gpio_cfgs[1],
		.chip   = {
			.base   = S5P6450_GPH(0),
			.ngpio  = S5P6450_GPIO_H_NR,
			.to_irq	= s3c_gph_to_irq,
			.label  = "GPH",
		},
	},
};

static int s3c_gpr_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return IRQ_EINT_GROUP(8, offset + 8);
}

static struct s3c_gpio_chip gpio_rbank_4bit2[] = {
	{
		.base	= S5P6450_GPR_BASE + 0x4,
		.config	= &s5p6450_gpio_cfgs[2],
		.chip	= {
			.base	= S5P6450_GPR(0),
			.ngpio	= S5P6450_GPIO_R_NR,
			.to_irq	= s3c_gpr_to_irq,
			.label	= "GPR",
		},
	},
};

static int s3c_gpn_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return S5P_EINT(offset);
}

static int s3c_gpp_to_irq(struct gpio_chip *chip, unsigned offset)
{
	if (offset <= 2)	/* GPP0..2 */
		return -EINVAL;
	if (offset == 12)	/* GPP12 */
		return -EINVAL;
	return IRQ_EINT_GROUP(8, offset);
}

static struct s3c_gpio_chip s5p6450_gpio_2bit[] = {
	{
		.base	= S5P6450_GPF_BASE,
		.config	= &s5p6450_gpio_cfgs[5],
		.chip	= {
			.base	= S5P6450_GPF(0),
			.ngpio	= S5P6450_GPIO_F_NR,
			.label	= "GPF",
		},
	}, {
		.base	= S5P6450_GPI_BASE,
		.config	= &s5p6450_gpio_cfgs[3],
		.chip	= {
			.base	= S5P6450_GPI(0),
			.ngpio	= S5P6450_GPIO_I_NR,
			.label	= "GPI",
		},
	}, {
		.base	= S5P6450_GPJ_BASE,
		.config	= &s5p6450_gpio_cfgs[3],
		.chip	= {
			.base	= S5P6450_GPJ(0),
			.ngpio	= S5P6450_GPIO_J_NR,
			.label	= "GPJ",
		},
	}, {
		.base	= S5P6450_GPN_BASE,
		.config	= &s5p6450_gpio_cfgs[4],
		.chip	= {
			.base	= S5P6450_GPN(0),
			.ngpio	= S5P6450_GPIO_N_NR,
			.to_irq	= s3c_gpn_to_irq,
			.label	= "GPN",
		},
	}, {
		.base	= S5P6450_GPP_BASE,
		.config	= &s5p6450_gpio_cfgs[5],
		.chip	= {
			.base	= S5P6450_GPP(0),
			.ngpio	= S5P6450_GPIO_P_NR,
			.to_irq	= s3c_gpp_to_irq,
			.label	= "GPP",
		},
	}, {
		.base   = S5P6450_GPQ_BASE,
		.config = &s5p6450_gpio_cfgs[4],
		.chip   = {
			.base   = S5P6450_GPQ(0),
			.ngpio  = S5P6450_GPIO_Q_NR,
			.label  = "GPQ",
		},
	    }, {
		.base   = S5P6450_GPS_BASE,
		.config = &s5p6450_gpio_cfgs[5],
		.chip   = {
			.base   = S5P6450_GPS(0),
			.ngpio  = S5P6450_GPIO_S_NR,
			.label  = "GPS",
		},
	},
};

void __init s5p6450_gpiolib_set_cfg(struct s3c_gpio_cfg *chipcfg, int nr_chips)
{
	for (; nr_chips > 0; nr_chips--, chipcfg++) {
		if (!chipcfg->set_config)
			chipcfg->set_config	= s3c_gpio_setcfg_s3c64xx_4bit;
		if (!chipcfg->set_pull)
			chipcfg->set_pull	= s3c_gpio_setpull_updown;
		if (!chipcfg->get_pull)
			chipcfg->get_pull	= s3c_gpio_getpull_updown;
	}
}

static void __init s5p6450_gpio_add_rbank_4bit2(struct s3c_gpio_chip *chip,
						int nr_chips)
{
	for (; nr_chips > 0; nr_chips--, chip++) {
		chip->chip.direction_input = s5p6450_gpiolib_rbank_4bit2_input;
		chip->chip.direction_output =
					s5p6450_gpiolib_rbank_4bit2_output;
		s3c_gpiolib_add(chip);
	}
}

static int __init s5p6450_gpiolib_init(void)
{
	struct s3c_gpio_chip *chips = s5p6450_gpio_2bit;
	int nr_chips = ARRAY_SIZE(s5p6450_gpio_2bit);

	s5p6450_gpiolib_set_cfg(s5p6450_gpio_cfgs,
				ARRAY_SIZE(s5p6450_gpio_cfgs));

	for (; nr_chips > 0; nr_chips--, chips++)
		s3c_gpiolib_add(chips);

	samsung_gpiolib_add_4bit_chips(s5p6450_gpio_4bit,
				ARRAY_SIZE(s5p6450_gpio_4bit));

	samsung_gpiolib_add_4bit2_chips(s5p6450_gpio_4bit2,
				ARRAY_SIZE(s5p6450_gpio_4bit2));

	s5p6450_gpio_add_rbank_4bit2(gpio_rbank_4bit2,
				ARRAY_SIZE(gpio_rbank_4bit2));

	return 0;
}
#ifdef CONFIG_GPIO_VGPIO_EARLY
postcore_initcall(s5p6450_gpiolib_init);
#else
arch_initcall(s5p6450_gpiolib_init);
#endif
