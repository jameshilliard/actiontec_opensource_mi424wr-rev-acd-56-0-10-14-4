/****************************************************************************
 *
 * rg/vendor/actiontec/ri408wr/modules/plf_leds.c
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

#define INVERSE_LED (BIT(31))
#define MASK_LED(led) (led & ~INVERSE_LED)
#define IS_INVERSE_LED(led) (led & INVERSE_LED)

static u32 ri408wr_leds_led_to_gpio[] = 
{
    [LED_ALARM] = IXP425_GPIO_PIN_1,
    [LED_POWER] = IXP425_GPIO_PIN_2,
    [LED_WIRELESS] = INVERSE_LED | IXP425_GPIO_PIN_3,
    [LED_INET_DOWN] = IXP425_GPIO_PIN_4,
    [LED_INET_OK] = IXP425_GPIO_PIN_5,
    [LED_MALFUNC] = IXP425_GPIO_PIN_6,
    [LED_STAND_BY] = IXP425_GPIO_PIN_9,
};

static int ri408wr_leds_init(void) 
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

static void ri408wr_leds_cleanup(void) 
{ 
}

static int ri408wr_leds_op(int led, int state)
{
    u32 led_bitmask = ri408wr_leds_led_to_gpio[led];
    u32 val = IS_INVERSE_LED(led_bitmask) ? !state : state;
    u32 pin = MASK_LED(led_bitmask);
    gpio_line_config(pin, IXP425_GPIO_OUT);
    gpio_line_set(pin, val);

#ifdef DEBUG
    printk("leds: gpio[%d] is %s\n", pin, state ? "on" : "off");
#endif

    return 0;
}

struct platform_led_ops_t platform_led_ops = {
    .plf_leds_init = ri408wr_leds_init,
    .plf_leds_cleanup = ri408wr_leds_cleanup,
    .plf_leds_op = ri408wr_leds_op
};

