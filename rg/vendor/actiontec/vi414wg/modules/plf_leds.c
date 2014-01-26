/****************************************************************************
 *
 * rg/vendor/actiontec/vi414wg/modules/plf_leds.c
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

#include <linux/types.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#include "kleds.h"
#include "vendor/latch.h"

#define INVERSE_LED (BIT(31))
#define GPIO_LED (BIT(30))
#define MASK_LED(led) (led & ~(INVERSE_LED | GPIO_LED))
#define IS_INVERSE_LED(led) (led & INVERSE_LED)
#define IS_GPIO_LED(led) (led & GPIO_LED)

static u32 vi414wg_leds_led_to_bitmask_map[] = 
{
    [LED_ALARM] = VI414WG_LATCH_ALARM_LED,
    [LED_POWER] = VI414WG_LATCH_POWER_LED,
    [LED_WIRELESS] = INVERSE_LED | VI414WG_LATCH_WIRELESS_LED,
    [LED_INET_DOWN] = VI414WG_LATCH_INTET_DOWN_LED,
    [LED_INET_OK] = VI414WG_LATCH_INTET_OK_LED,
    [LED_PHONE1] = INVERSE_LED | VI414WG_LATCH_PHONE1_LED,
    [LED_PHONE2] = INVERSE_LED | VI414WG_LATCH_PHONE2_LED,
    [LED_VOIP] = INVERSE_LED | VI414WG_LATCH_VOIP_LED,
    [LED_MOCA_LAN] = VI414WG_LATCH_MOCA_LAN_LED,
};

static int vi414wg_leds_init(void) 
{
    kleds_req_t power_req = {
	.led = LED_POWER,
	.op = LED_OP_BLINKING_START,
	.duty_cycle = 250,
	.period = 500,
	.count = BLINK_FOREVER,
    };

    return kleds_request(&power_req);
}

static void vi414wg_leds_cleanup(void) 
{ 
}

static int vi414wg_leds_op(int led, int state)
{
    u32 led_bitmask = vi414wg_leds_led_to_bitmask_map[led];
    u32 pin = MASK_LED(led_bitmask);
    u32 val = IS_INVERSE_LED(led_bitmask) ? !state : state;

    if (IS_GPIO_LED(led_bitmask))
    {
	gpio_line_config(pin, IXP425_GPIO_OUT);
	gpio_line_set(pin, val);
    }
    else
	vi414wg_latch_set(pin, val);

#ifdef DEBUG
    printk("leds: %s[%d] is %s\n",
	IS_GPIO_LED(led_bitmask) ? "gpio" : "latch", pin, val ? "on" : "off");
#endif

    return 0;
}

struct platform_led_ops_t platform_led_ops = {
    .plf_leds_init = vi414wg_leds_init,
    .plf_leds_cleanup = vi414wg_leds_cleanup,
    .plf_leds_op = vi414wg_leds_op
};

