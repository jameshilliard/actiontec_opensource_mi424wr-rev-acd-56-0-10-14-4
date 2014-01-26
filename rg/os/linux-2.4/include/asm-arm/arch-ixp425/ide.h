/*
 *  linux/include/asm-arm/arch-ixp425/ide.h
 *
 *  Copyright (C) 1994-1996  Linus Torvalds & authors
 *
 * derived from:
 * linux/include/asm-arm/arch-shark/ide.h
 * Copyright (c) 2003 Barry Dallavalle
 */

#ifndef _ARCH_IXP425_IDE_H_
#define _ARCH_IXP425_IDE_H_

#if defined(__KERNEL__) && defined(CONFIG_ARCH_IXP425_COYOTE)

#include <asm/arch/ixp425.h>
#include <asm/arch/gpio.h>
#include <asm/arch/irqs.h>

#define CUSTOM_IDE_IO
#undef MAX_HWIFS
#define MAX_HWIFS 1 /* Only one HWIF */

#define IDE_REG(p) *((volatile u16 *)(IXP425_EXP_BUS_CS3_BASE_VIRT + (p)))

static __inline__ u8 custom_ide_inb(unsigned long port)
{
    return (u8)IDE_REG(port);
}

static __inline__ u16 custom_ide_inw(unsigned long port)
{
    return IDE_REG(port);
}

static __inline__ void custom_ide_insw(unsigned long port, void *addr,
    u32 count)
{
    u16 *dst = (u16 *)addr;

    while (count--)
	*dst++ = IDE_REG(port);
}

static __inline__ u32 custom_ide_inl(unsigned long port)
{
    printk("ide_inl not implemented");
    BUG();

    return 0;
}

static __inline__ void custom_ide_insl(unsigned long port, void *addr,
    u32 count)
{
    custom_ide_insw(port, addr, count << 1);
}

static __inline__ void custom_ide_outb(u8 val, unsigned long port)
{
    IDE_REG(port) = (u16)val;
}

#define custom_ide_outbsync(d,v,p) custom_ide_outb(v,p)

static __inline__ void custom_ide_outw(u16 val, unsigned long port)
{
    IDE_REG(port) = val;
}

static __inline__ void custom_ide_outsw(unsigned long port, void *addr,
    u32 count)
{
    u16 *dst = addr;

    while (count--)
	IDE_REG(port) = *dst++;
}

static __inline__ void custom_ide_outl(u32 val, unsigned long port)
{
    printk("ide_outl not implemented");
    BUG();
}

static __inline__ void custom_ide_outsl(unsigned long port, void *addr,
    u32 count)
{
    custom_ide_outsw(port, addr, count << 1);
}

static __inline__ void ide_init_hwif_ports(hw_regs_t *hw,
    ide_ioreg_t data_port, ide_ioreg_t ctrl_port, int *irq)
{
    ide_ioreg_t reg = data_port;
    int i;

    /* Allow 16 bit RW access to expansion bus chipset 3 */
    *IXP425_EXP_CS3 = 0xbfff0002;

    gpio_line_config(IXP425_GPIO_PIN_5,
	IXP425_GPIO_IN | IXP425_GPIO_ACTIVE_HIGH);
    gpio_line_isr_clear(IXP425_GPIO_PIN_5);

    for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++)
    {
	hw->io_ports[i] = reg;
	reg += 2;
    }
    hw->io_ports[IDE_CONTROL_OFFSET] = ctrl_port;
    if (irq != NULL)
	*irq = 0;
}

static __inline__ void ide_init_default_hwifs(void)
{
    hw_regs_t hw;

    ide_init_hwif_ports(&hw, 0xe0, 0xfc, NULL);
    hw.irq = IRQ_IXP425_GPIO5;
    ide_register_hw(&hw, NULL);
}

#else

#include <asm/irq.h>

/*
 * Set up a hw structure for a specified data port, control port and IRQ.
 * This should follow whatever the default interface uses.
 */
static __inline__ void
ide_init_hwif_ports(hw_regs_t *hw, int data_port, int ctrl_port, int *irq)
{
	ide_ioreg_t reg = (ide_ioreg_t) data_port;
	int i;

	for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++) {
		hw->io_ports[i] = reg;
		reg += 1;
	}
	hw->io_ports[IDE_CONTROL_OFFSET] = (ide_ioreg_t) ctrl_port;
	if (irq)
		*irq = 0;
}

/*
 * This registers the standard ports for this architecture with the IDE
 * driver.
 */
static __inline__ void ide_init_default_hwifs(void)
{
	/* There are no standard ports */
}

#endif /* KERNEL && CONFIG_ARCH_IXP425_COYOTE */

#endif /* _ARCH_IXP425_IDE_H_ */
