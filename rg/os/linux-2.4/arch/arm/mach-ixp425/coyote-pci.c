/****************************************************************************
 *
 * /home/bat/bat/actiontec_bhr_4_0/rg/os/linux-2.4/arch/arm/mach-ixp425/coyote-pci.c
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

/*
 * Coyote GPIO map:
 * 
 * 0	PWR_FAIL_N
 * 1	DSL_IRQ_N
 * 2	SUC_A_IRQ_N
 * 3	SUC_B_IRQ_N
 * 4	DSP_IRQ_N
 * 5	IDE_IRQ_N
 * 6	PCI2_IRQB_N
 * 7	SPI_CS1_N
 * 8	SPI_CS0_N
 * 9	SPI_SCK
 * 10	SPI_SDI
 * 11	PCI1_IRQA_N
 * 12	IO_RESET
 * 13	SPI_SDO
 * 14	PCI_CLK
 * 15	EXP_CLK
 * 
 */

#define IXP425_PCI_INTA_IRQ	IRQ_IXP425_GPIO11
#define IXP425_PCI_INTB_IRQ	IRQ_IXP425_GPIO6

/* PCI slots */
#define COYOTE_MINIPCI_0	15
#define COYOTE_MINIPCI_1	14

#ifdef CONFIG_PCI_RESET

#define IXP425_PCI_INTA_GPIO	IXP425_GPIO_PIN_11
#define IXP425_PCI_INTB_GPIO	IXP425_GPIO_PIN_6

/* PCI controller pin mappings */
#define IXP425_PCI_RESET_GPIO   IXP425_GPIO_PIN_12
#define IXP425_PCI_CLK_PIN      IXP425_GPIO_CLK_0
#define IXP425_PCI_CLK_ENABLE   IXP425_GPIO_CLK0_ENABLE
#define IXP425_PCI_CLK_TC_LSH   IXP425_GPIO_CLK0TC_LSH
#define IXP425_PCI_CLK_DC_LSH   IXP425_GPIO_CLK0DC_LSH

void __init coyote_pci_hw_init(void)
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

void __init coyote_pci_init(void *sysdata)
{
#ifdef CONFIG_PCI_RESET
	if (ixp425_pci_is_host())
		coyote_pci_hw_init();
#endif
	ixp425_pci_init(sysdata);
}

static int __init coyote_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
    int irq = -1;

    /* If device doesn't require interrupt or requests
     * illigal PCI interrupt pin
     */
    if (pin < 1 || pin > 4)
	goto Exit;
	
    switch(slot)
    {
    case COYOTE_MINIPCI_0:
	irq = IXP425_PCI_INTA_IRQ;
	break;

    case COYOTE_MINIPCI_1:
	irq = IXP425_PCI_INTB_IRQ;
	break;
    }

Exit:
    printk("\n>> PCI IRQ %d.%d : %d\n", slot, pin, irq);

    return irq;
}

struct hw_pci coyote_pci __initdata = {
	init:		coyote_pci_init,
	swizzle:	common_swizzle,
	map_irq:	coyote_map_irq,
};

