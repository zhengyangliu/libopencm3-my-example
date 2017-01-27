/**
 * @file blink.c
 * @synopsis  
 * @author Yangliu, 869705086@qq.com
 * @version     V1.1
 * @date 2017-01-25
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "delay.h"

/* --------------------------------------------------------------------------*/
/**
 * @synopsis  gpio_setup     
 */
/* ----------------------------------------------------------------------------*/
static void gpio_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOG);
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);
}

/* Set STM32 system clock to 168 MHz. */
static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

int main(void)
{
	clock_setup();
	gpio_setup();
	delay_setup();

	while (1)
	{
		gpio_toggle(GPIOG, GPIO13);	/* LED on/off */
		delay_ms(1000);
	}
    
	return 0;
}

