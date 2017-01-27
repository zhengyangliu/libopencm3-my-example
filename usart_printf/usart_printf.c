/**
 * @file usart.c
 * @Brief  cm3 串口例程，截取自cm3例程，功能是流水灯+串口发送数字.
 *         在printf结尾上必须使用\n 或者fflush(stdout);清空缓存发
 *         送数据，否则后续的printf操作会将之前的数据覆盖，且在不
 *         使用前两者的方式下printf不会输出任何的数据内容。
 * @author Yangliu, 869705086@qq.com
 * @version 1.1
 * @date 2017-01-25
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <stdlib.h>

int _write(int fd, char *ptr, int len);

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

	printf("\nStandard printf example.\n");

	while (1)
	{

		gpio_toggle(GPIOG, GPIO13);         	/* LED on/off */
		c = (c == 9) ? 0 : c + 1;	            /* Increment c. */

		/* Method one */
		/*printf("%d\n",c);*/

		/* Method two */
		printf("%d", c);
		fflush(stdout);

		if ((++j % 10) == 0)	 /* Newline after line full. */
		{
			printf("\n");
		}

		for (i = 0; i < 300000; i++)  	/* Wait a bit. */
		{
			__asm__("NOP");
		}
	}

	return 0;
}

int _write(int fd, char *ptr, int len)
{
	int i = 0;

	/*
	 * write "len" of char from "ptr" to file id "fd"
	 * Return number of char written.
	 *
	* Only work for STDOUT, STDIN, and STDERR
	 */
	if (fd > 2)
	{
		return -1;
	}

	while (*ptr && (i < len))
	{
		usart_send_blocking(USART1, *ptr);

		if (*ptr == '\n')
		{
			usart_send_blocking(USART1, '\r');
		}

		i++;
		ptr++;
	}

	return i;
}
