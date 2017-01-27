/*
 * 使用systick的延时函数
 * 截取自libopencm3 project
 * 时间 ： 2017.1.24
 * 作者 ： Yangliu
 *
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "delay.h"

volatile uint32_t system_millis;

/* Called when systick fires */
void sys_tick_handler(void)
{
	system_millis++;
}

/* sleep for delay milliseconds */
void delay_ms(uint32_t delay)
{
	uint32_t wake = system_millis + delay;

	while (wake > system_millis);
}

/*
 * systick_setup(void)
 *
 * This function sets up the 1khz "system tick" count. The SYSTICK counter is a
 * standard feature of the Cortex-M series.
 */
void delay_setup(void)
{
	/* clock rate / 1000 to get 1mS interrupt rate */
	systick_set_reload(168000);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	/* this done last */
	systick_interrupt_enable();
}
