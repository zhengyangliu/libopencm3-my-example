/**
 * @file usart.c
 * @Brief  cm3 串口例程，截取自cm3例程，功能是流水灯+串口发送数字
 * @author Yangliu, 869705086@qq.com
 * @version 1.1 
 * @date 2017-01-25
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

static void clock_setup(void)
{
	/* Enable GPIOG clock for LED & USARTs. */
    rcc_periph_clock_enable(RCC_GPIOG);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART1. */
	rcc_periph_clock_enable(RCC_USART1);
}

static void usart_setup(void)
{
	/* Setup USART1 parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);
}

static void gpio_setup(void)
{
	/* Setup GPIO pin GPIO13 on GPIO port G for LED. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	/* Setup GPIO pins for USART1 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	/* Setup USART1  pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
}

int main(void)
{
	int i, j = 0, c = 0;

	clock_setup();
	gpio_setup();
	usart_setup();

	while (1)
	{
		gpio_toggle(GPIOG, GPIO13);         	/* LED on/off */
		usart_send_blocking(USART1, c + '0');   /* USART1: Send byte. */
		c = (c == 9) ? 0 : c + 1;	/* Increment c. */

		if ((++j % 80) == 0)  		/* Newline after line full. */
		{
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
		}

		for (i = 0; i < 3000000; i++)  	/* Wait a bit. */
		{
			__asm__("NOP");
		}
	}

	return 0;
}
