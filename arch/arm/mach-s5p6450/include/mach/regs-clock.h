/* linux/arch/arm/mach-s5p6450/include/mach/regs-clock.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5P6450 - Clock register definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_REGS_CLOCK_H
#define __ASM_ARCH_REGS_CLOCK_H __FILE__

#include <mach/map.h>

#define S5P_CLKREG(x)		(S3C_VA_SYS + (x))

#define S5P_APLL_LOCK		S5P_CLKREG(0x00)
#define S5P_MPLL_LOCK		S5P_CLKREG(0x04)
#define S5P_EPLL_LOCK		S5P_CLKREG(0x08)
#define S5P_APLL_CON		S5P_CLKREG(0x0C)
#define S5P_MPLL_CON		S5P_CLKREG(0x10)
#define S5P_EPLL_CON		S5P_CLKREG(0x14)
#define S5P_EPLL_CON_K		S5P_CLKREG(0x18)
#define S5P_CLK_SRC0		S5P_CLKREG(0x1C)
#define S5P_CLK_DIV0		S5P_CLKREG(0x20)
#define S5P_CLK_DIV1		S5P_CLKREG(0x24)
#define S5P_CLK_DIV2		S5P_CLKREG(0x28)
#define S5P_CLK_OUT		S5P_CLKREG(0x2C)
#define S5P_CLK_GATE_HCLK0	S5P_CLKREG(0x30)
#define S5P_CLK_GATE_PCLK	S5P_CLKREG(0x34)
#define S5P_CLK_GATE_SCLK0	S5P_CLKREG(0x38)
#define S5P_CLK_GATE_MEM0	S5P_CLKREG(0x3C)
#define S5P_CLK_DIV3		S5P_CLKREG(0x40)
#define S5P_CLK_GATE_HCLK1	S5P_CLKREG(0x44)
#define S5P_CLK_GATE_SCLK1	S5P_CLKREG(0x48)
#define S5P_CLK_OUT1		S5P_CLKREG(0x4C)
#define S5P_DPLL_CON		S5P_CLKREG(0x50)
#define S5P_DPLL_CON_K		S5P_CLKREG(0x54)
#define S5P_DPLL_LOCK		S5P_CLKREG(0x58)
#define S5P_CLK_SRC1           	S5P_CLKREG(0x10C)
#define S5P_SWRESET		S5P_CLKREG(0x114)
#define S5P_SYS_ID		S5P_CLKREG(0x118)
#define S5P_SYS_OTHERS		S5P_CLKREG(0x11C)
#define S5P_MEM_CFG_STAT	S5P_CLKREG(0x12C)
#define S5P_PWR_CFG		S5P_CLKREG(0x804)
#define S5P_EINT_WAKEUP_MASK	S5P_CLKREG(0x808)
#define S5P_STOP_CFG		S5P_CLKREG(0x814)
#define S5P_SLEEP_CFG		S5P_CLKREG(0x818)
#define S5P_OSC_FREQ		S5P_CLKREG(0x820)
#define S5P_OSC_STABLE		S5P_CLKREG(0x824)
#define S5P_PWR_STABLE		S5P_CLKREG(0x828)
#define S5P_MTC_STABLE		S5P_CLKREG(0x830)
#define S5P_OTHERS		S5P_CLKREG(0x900)
#define S5P_RST_STAT		S5P_CLKREG(0x904)
#define S5P_WAKEUP_STAT		S5P_CLKREG(0x908)


#define S5P_INFORM0		S5P_CLKREG(0xA00)
#define S5P_INFORM1		S5P_CLKREG(0xA04)
#define S5P_INFORM2		S5P_CLKREG(0xA08)
#define S5P_INFORM3		S5P_CLKREG(0xA0C)
#define S5P_INFORM4		S5P_CLKREG(0xA10)

#define S5P_SLEEP_CFG_OSC_EN            (1 << 0)

#define S5P_CFG_WFI_CLEAN              (~(3 << 5))
#define S5P_CFG_WFI_IDLE                (1 << 5)
#define S5P_CFG_WFI_STOP                (2 << 5)
#define S5P_CFG_WFI_SLEEP               (3 << 5)

#define S5P6450_PWRCFG_CFG_WFI_MASK     (0x3 << 5)
#define S5P6450_PWRCFG_CFG_WFI_IGNORE   (0x0 << 5)
#define S5P6450_PWRCFG_CFG_WFI_IDLE     (0x1 << 5)
#define S5P6450_PWRCFG_CFG_WFI_STOP     (0x2 << 5)
#define S5P6450_PWRCFG_CFG_WFI_SLEEP    (0x3 << 5)



#define S5P_OTHER_SYSC_INTOFF           (1 << 31)

#define S5P_EPLL_CON_M_SHIFT    16
#define S5P_EPLL_CON_P_SHIFT    8
#define S5P_EPLL_CON_S_SHIFT    0
#define S5P_EPLL_CON_K_SHIFT    0

#define S5P_DPLL_CON_M_SHIFT    16
#define S5P_DPLL_CON_P_SHIFT    8
#define S5P_DPLL_CON_S_SHIFT    0
#define S5P_DPLL_CON_K_SHIFT    0

/* CLKDIV0 */
#define S5P_CLKDIV0_PCLK83_MASK		(0xf << 12)
#define S5P_CLKDIV0_PCLK83_SHIFT	(12)
#define S5P_CLKDIV0_HCLK166_MASK	(0xf << 8)
#define S5P_CLKDIV0_HCLK166_SHIFT	(8)
#define S5P_CLKDIV0_MPLL_MASK		(0x1 << 4)
#define S5P_CLKDIV0_MPLL_SHIFT		(4)
#define S5P_CLKDIV0_ARM_MASK		(0xf << 0)
#define S5P_CLKDIV0_ARM_SHIFT		(0)

/* CLKDIV1 */
#define S5P_CLKDIV1_MMC44_MASK          (0xf << 28)
#define S5P_CLKDIV1_MMC44_SHIFT         (28)
#define S5P_CLKDIV1_EPLL_MASK           (0xf << 24)
#define S5P_CLKDIV1_EPLL_SHIFT          (24)
#define S5P_CLKDIV1_CAMIF_MASK          (0xf << 20)
#define S5P_CLKDIV1_CAMIF_SHIFT         (20)
#define S5P_CLKDIV1_USI_MASK            (0xf << 16)
#define S5P_CLKDIV1_USI_SHIFT           (16)
#define S5P_CLKDIV1_FIMC_MASK           (0xf << 12)
#define S5P_CLKDIV1_FIMC_SHIFT          (12)
#define S5P_CLKDIV1_MMC2_MASK           (0xf << 8)
#define S5P_CLKDIV1_MMC2_SHIFT          (8)
#define S5P_CLKDIV1_MMC1_MASK           (0xf << 4)
#define S5P_CLKDIV1_MMC1_SHIFT          (4)
#define S5P_CLKDIV1_MMC0_MASK           (0xf << 0)
#define S5P_CLKDIV1_MMC0_SHIFT          (0)

/* CLKDIV2 */
//#define S5P_CLKDIV2_AUDIO2_MASK         (0xf << 24)
//#define S5P_CLKDIV2_AUDIO2_SHIFT        (24)
#define S5P_CLKDIV2_2D_MASK		(0xf << 20)
#define S5P_CLKDIV2_2D_SHIFT		(20)
#define S5P_CLKDIV2_UART_MASK           (0xf << 16)
#define S5P_CLKDIV2_UART_SHIFT          (16)
//#define S5P_CLKDIV2_AUDIO1_MASK         (0xf << 12)
//#define S5P_CLKDIV2_AUDIO1_SHIFT        (12)
//#define S5P_CLKDIV2_AUDIO0_MASK         (0xf << 8)
//#define S5P_CLKDIV2_AUDIO0_SHIFT        (8)
#define S5P_CLKDIV2_SPI1_MASK           (0xf << 4)
#define S5P_CLKDIV2_SPI1_SHIFT          (4)
#define S5P_CLKDIV2_SPI0_MASK           (0xf << 0)
#define S5P_CLKDIV2_SPI0_SHIFT          (0)


/* CLKDIV3 */
/*
#define S5P_CLKDIV3_PCLK_LOW_MASK	(0xf << 12)
#define S5P_CLKDIV3_PCLK_LOW_SHIFT	(12)
#define S5P_CLKDIV3_HCLK_LOW_MASK	(0xf << 8)
#define S5P_CLKDIV3_HCLK_LOW_SHIFT	(8)
*/
#define S5P_CLKDIV3_PCLK_TO_WDT_PWM1_MASK	(0xf << 20)
#define S5P_CLKDIV3_PCLK_TO_WDT_PWM1_SHIFT	(20)
#define S5P_CLKDIV3_PCLK_TO_WDT_PWM0_MASK	(0xf << 16)
#define S5P_CLKDIV3_PCLK_TO_WDT_PWM0_SHIFT	(16)
#define S5P_CLKDIV3_PCLK66_MASK			(0xf << 12)
#define S5P_CLKDIV3_PCLK66_SHIFT		(12)
#define S5P_CLKDIV3_HCLK133_MASK		(0xf << 8)
#define S5P_CLKDIV3_HCLK133_SHIFT		(8)
#define S5P_CLKDIV3_MALI_MASK			(0xf << 4)
#define S5P_CLKDIV3_MALI_SHIFT			(4)
#define S5P_CLKDIV3_DISPCON_MASK		(0xf << 0)
#define S5P_CLKDIV3_DISPCON_SHIFT		(0)

/* HCLK0 GATE Registers */
#define S5P_CLKCON_HCLK0_IROM		(1<<25)
#define S5P_CLKCON_HCLK0_MEM0		(1<<21)
#define S5P_CLKCON_HCLK0_USB		(1<<20)
#define S5P_CLKCON_HCLK0_HSMMC2		(1<<19)
#define S5P_CLKCON_HCLK0_HSMMC1		(1<<18)
#define S5P_CLKCON_HCLK0_HSMMC0		(1<<17)
#define S5P_CLKCON_HCLK0_HSMMC44	(1<<16)
#define S5P_CLKCON_HCLK0_DMA0		(1<<12)
#define S5P_CLKCON_HCLK0_2D		(1<<8)
#define S5P_CLKCON_HCLK0_ROTATOR	(1<<7)
#define S5P_CLKCON_HCLK0_TZIC1		(1<<6)
#define S5P_CLKCON_HCLK0_FIMC		(1<<5)
#define S5P_CLKCON_HCLK0_SSS		(1<<4)
#define S5P_CLKCON_HCLK0_HOST		(1<<3)
#define S5P_CLKCON_HCLK0_TZIC0		(1<<2)
#define S5P_CLKCON_HCLK0_INTC		(1<<1)

/* HCLK1 GATE Registers */
#define S5P_CLKCON_HCLK1_DISPCON	(1<<1)
#define S5P_CLKCON_HCLK1_TSI		(1<<0)

/* PCLK GATE Registers */
#define S5P_CLKCON_PCLK_2D		(1<<31)
#define S5P_CLKCON_PCLK_DMC0		(1<<30)
#define S5P_CLKCON_PCLK_ETM		(1<<29)
#define S5P_CLKCON_PCLK_DAP		(1<<28)
#define S5P_CLKCON_PCLK_IIC1		(1<<27)
#define S5P_CLKCON_PCLK_I2S0		(1<<26)
#define S5P_CLKCON_PCLK_TZPC0		(1<<25)
#define S5P_CLKCON_PCLK_TZPC1		(1<<24)
#define S5P_CLKCON_PCLK_DSIM		(1<<23)
#define S5P_CLKCON_PCLK_SPI1		(1<<22)
#define S5P_CLKCON_PCLK_SPI0		(1<<21)
#define S5P_CLKCON_PCLK_SECJTAG		(1<<20)
#define S5P_CLKCON_PCLK_SECKEY		(1<<19)
#define S5P_CLKCON_PCLK_GPIO		(1<<18)
#define S5P_CLKCON_PCLK_IIC0		(1<<17)
#define S5P_CLKCON_PCLK_I2S2		(1<<16)
#define S5P_CLKCON_PCLK_I2S1		(1<<15)
#define S5P_CLKCON_PCLK_ROTATOR		(1<<14)
#define S5P_CLKCON_PCLK_USI		(1<<13)
#define S5P_CLKCON_PCLK_TSADC		(1<<12)
#define S5P_CLKCON_PCLK_PCM1		(1<<11)
#define S5P_CLKCON_PCLK_PCM2		(1<<10)
#define S5P_CLKCON_PCLK_UART4		(1<<9)
#define S5P_CLKCON_PCLK_PCM0		(1<<8)
#define S5P_CLKCON_PCLK_PWM		(1<<7)
#define S5P_CLKCON_PCLK_RTC		(1<<6)
#define S5P_CLKCON_PCLK_WDT		(1<<5)
#define S5P_CLKCON_PCLK_UART3		(1<<4)
#define S5P_CLKCON_PCLK_UART2		(1<<3)
#define S5P_CLKCON_PCLK_UART1		(1<<2)
#define S5P_CLKCON_PCLK_UART0		(1<<1)
#define S5P_CLKCON_PCLK_UART5		(1<<0)

/* SCLK0 GATE Registers */
#define S5P_CLKCON_SCLK0_MMC44		(1<<30)
#define S5P_CLKCON_SCLK0_MMC2_48	(1<<29)
#define S5P_CLKCON_SCLK0_MMC1_48	(1<<28)
#define S5P_CLKCON_SCLK0_MMC0_48	(1<<27)
#define S5P_CLKCON_SCLK0_MMC2		(1<<26)
#define S5P_CLKCON_SCLK0_MMC1		(1<<25)
#define S5P_CLKCON_SCLK0_MMC0		(1<<24)
#define S5P_CLKCON_SCLK0_SPI1_48 	(1<<23)
#define S5P_CLKCON_SCLK0_SPI0_48 	(1<<22)
#define S5P_CLKCON_SCLK0_SPI1		(1<<21)
#define S5P_CLKCON_SCLK0_SPI0		(1<<20)
#define S5P_CLKCON_SCLK0_2D		(1<<12)
#define S5P_CLKCON_SCLK0_AUDIO2		(1<<11)
#define S5P_CLKCON_SCLK0_FIMC		(1<<10)
#define S5P_CLKCON_SCLK0_AUDIO1		(1<<9)
#define S5P_CLKCON_SCLK0_AUDIO0		(1<<8)
#define S5P_CLKCON_SCLK0_USI		(1<<7)
#define S5P_CLKCON_SCLK0_CAMIF		(1<<6)
#define S5P_CLKCON_SCLK0_UART		(1<<5)
#define S5P_CLKCON_SCLK0_DISPCON	(1<<1)

/* SCLK1 GATE Registers */
#define S5P_CLKCON_SCLK1_MALI		(1<<2)
#define S5P_CLKCON_SCLK1_DISPCON	(1<<1)

/* MEM0 GATE Registers */
#define S5P_CLKCON_MEM0_HCLK_NFCON	(1<<2)
#define S5P_CLKCON_MEM0_HCLK_SROM	(1<<1)
#define S5P_CLKCON_MEM0_HCLK_DMC0	(1<<0)

/*OTHERS Resgister */
#define S5P_OTHERS_USB_SIG_MASK		(1<<16)
#define S5P_OTHERS_HCLK_LOW_SEL_MPLL	(1<<6)

/* Compatibility defines */
#define ARM_CLK_DIV			S5P_CLK_DIV0
#define ARM_DIV_RATIO_SHIFT		0
#define ARM_DIV_MASK			(0xf << ARM_DIV_RATIO_SHIFT)

#endif /* __ASM_ARCH_REGS_CLOCK_H */
