/****************************************************************************
 *
 * /home/bat/bat/actiontec_bhr_4_0/rg/os/linux-2.4/arch/arm/mach-ixp425/wx5715-pci.c
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */

#include <linux/pci.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <asm/mach/pci.h>
#include <asm/arch/irqs.h>
#include <asm/arch/pci.h>
#include <asm/arch/gpio.h>
#include <asm/arch/ixp425-pci.h>

#define IXP425_PCI_INTA_IRQ	IRQ_IXP425_GPIO11
#define IXP425_PCI_INTB_IRQ	IRQ_IXP425_GPIO10

#define SLOT_1	1
#define SLOT_2	2

#ifdef CONFIG_PCI_RESET

#define IXP425_PCI_INTA_GPIO	IXP425_GPIO_PIN_10
#define IXP425_PCI_INTB_GPIO	IXP425_GPIO_PIN_11

/* PCI controller pin mappings */
#define IXP425_PCI_RESET_GPIO	IXP425_GPIO_PIN_13
#define IXP425_PCI_CLK_PIN	IXP425_GPIO_CLK_0
#define IXP425_PCI_CLK_ENABLE	IXP425_GPIO_CLK0_ENABLE
#define IXP425_PCI_CLK_TC_LSH	IXP425_GPIO_CLK0TC_LSH
#define IXP425_PCI_CLK_DC_LSH	IXP425_GPIO_CLK0DC_LSH

void __init wx5715_pci_hw_init(void)
{
	/* Disable PCI clock */
	*IXP425_GPIO_GPCLKR &= ~IXP425_PCI_CLK_ENABLE;

	/* configure PCI-related GPIO */
	gpio_line_config(IXP425_PCI_CLK_PIN, IXP425_GPIO_OUT);
	gpio_line_config(IXP425_PCI_RESET_GPIO, IXP425_GPIO_OUT);

	gpio_line_config(IXP425_PCI_INTA_GPIO,
	    IXP425_GPIO_IN | IXP425_GPIO_ACTIVE_LOW);
	gpio_line_config(IXP425_PCI_INTB_GPIO,
	    IXP425_GPIO_IN | IXP425_GPIO_ACTIVE_LOW);

	gpio_line_isr_clear(IXP425_PCI_INTA_GPIO);
	gpio_line_isr_clear(IXP425_PCI_INTB_GPIO);

	/* Assert reset for PCI controller */
	gpio_line_set(IXP425_PCI_RESET_GPIO, IXP425_GPIO_LOW);
	/* wait 1ms to satisfy "minimum reset assertion time" of the PCI spec. */
	udelay(1000);
	/* Config PCI clock */
	*IXP425_GPIO_GPCLKR |= (0xf << IXP425_PCI_CLK_TC_LSH) | 
	    (0xf << IXP425_PCI_CLK_DC_LSH);
	/* Enable PCI clock */
	*IXP425_GPIO_GPCLKR |= IXP425_PCI_CLK_ENABLE;
	/* wait 100us to satisfy "minimum reset assertion time from clock stable"
	 * requirement of the PCI spec. */
	udelay(100);
	/* Deassert reset for PCI controller */
	gpio_line_set(IXP425_PCI_RESET_GPIO, IXP425_GPIO_HIGH);

	/* wait a while to let other devices get ready after PCI reset */
	udelay(1000);
}

#endif

void __init wx5715_pci_init(void *sysdata)
{
#ifdef CONFIG_PCI_RESET
	if (ixp425_pci_is_host())
		wx5715_pci_hw_init();
#endif
	ixp425_pci_init(sysdata);
}

static int __init wx5715_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
    switch (slot)
    {
    case SLOT_1:
	return IXP425_PCI_INTA_IRQ;

    case SLOT_2:
	return IXP425_PCI_INTB_IRQ;

    default:
	return -1;
    }

}

struct hw_pci wx5715_pci __initdata = {
	init:		wx5715_pci_init,
	swizzle:	common_swizzle,
	map_irq:	wx5715_map_irq,
};

