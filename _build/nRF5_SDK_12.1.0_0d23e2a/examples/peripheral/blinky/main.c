/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "bsp.h"
#include "drv_oled.h"

static const uint8_t leds_list[LEDS_NUMBER] = LEDS_LIST;

/**
 * @brief Function for application main entry.
 */
int main2(void)
{
    /* Configure LED-pins as outputs. */
    LEDS_CONFIGURE(LEDS_MASK);

    /* Toggle LEDs. */
    while (true)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            LEDS_INVERT(1 << leds_list[i]);
            nrf_delay_ms(500);
        }
    }
}
int main(void)
{
	drv_oled_init_t init_params;
	
	static const nrf_drv_twi_config_t twi_cfg =
	{
			.scl                = 19,
			.sda                = 20,
	};
	
	init_params.twi_addr=0x78;
	init_params.p_twi_cfg=&twi_cfg;
	drv_oled_init(&init_params);
	
	while(true)
	{
		drv_oled_test();
	}
}
/**
 *@}
 **/
