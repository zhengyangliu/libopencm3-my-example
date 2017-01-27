/**
 * @file usart.c
 * @Brief  cm3 串口例程,stdio输入输出函数
 * @author Yangliu, 869705086@qq.com
 * @version 1.1
 * @date 2017-01-25
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "delay.h"

/*
 * To implement the STDIO functions you need to create
 * the _read and _write functions and hook them to the
 * USART you are using. This example also has a buffered
 * read function for basic line editing.
 */
int _write(int fd, char *ptr, int len);
int _read(int fd, char *ptr, int len);
void get_buffered_line(void);

/*
 * This is a pretty classic ring buffer for characters
 */
#define BUFLEN 127

static uint16_t start_ndx;
static uint16_t end_ndx;
static char buf[BUFLEN + 1];
#define buf_len ((end_ndx - start_ndx) % BUFLEN)
static inline int inc_ndx(int n) { return ((n + 1) % BUFLEN); }
static inline int dec_ndx(int n) { return (((n + BUFLEN) - 1) % BUFLEN); }


static void clock_setup(void)
{
	/* Enable GPIOG clock for LED & USARTs. */
	rcc_periph_clock_enable(RCC_GPIOG);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART1. */
	rcc_periph_clock_enable(RCC_USART1);

    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

static void usart_setup(void)
{
	/* Setup USART1 parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
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
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);

	/* Setup USART1  pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);
}

int main(void)
{
	int i, j;

	clock_setup();
	gpio_setup();
	usart_setup();
    delay_setup();

	printf("\nStandard I/O Example.\n");

	/* Blink the LED on the board with every transmitted byte. */
	while (1)
	{
		int delay = 0;
		char local_buf[32];

		gpio_toggle(GPIOG, GPIO13);         	/* LED on/off */

		do
		{
			printf("Enter the delay(ms) constant for blink : ");
			fflush(stdout);
			fgets(local_buf, 32, stdin);
			delay = atoi(local_buf);

			if (delay <= 0)
			{
				printf("Error: expected a delay > 0\n");
			}
		}
		while (delay <= 0);

		printf("Blinking with a delay of %dms\n", delay);

		for (j = 0; j < 1000; j++)
		{
			gpio_toggle(GPIOG, GPIO13);

			for (i = 0; i < delay; i++)     /* Wait a bit. */
			{
				delay_ms(1);
			}
		}
	}

	return 0;
}

/* back up the cursor one space */
static inline void back_up(void)
{
	end_ndx = dec_ndx(end_ndx);
	usart_send_blocking(USART1, '\010');
	usart_send_blocking(USART1, ' ');
	usart_send_blocking(USART1, '\010');
}

/*
 * A buffered line editing function.
 */
void get_buffered_line(void)
{
	char c;

	if (start_ndx != end_ndx)
	{
		return;
	}

	while (1)
	{
		c = usart_recv_blocking(USART1);

		if (c == '\r')
		{
			buf[end_ndx] = '\n';
			end_ndx = inc_ndx(end_ndx);
			buf[end_ndx] = '\0';
			usart_send_blocking(USART1, '\r');
			usart_send_blocking(USART1, '\n');
			return;
		}

		/* or DEL erase a character */
		if ((c == '\010') || (c == '\177'))
		{
			if (buf_len == 0)
			{
				usart_send_blocking(USART1, '\a');
			}

			else
			{
				back_up();
			}

			/* erases a word */
		}

		else if (c == 0x17)
		{
			while ((buf_len > 0) &&
					(!(isspace((int) buf[end_ndx]))))
			{
				back_up();
			}

			/* erases the line */
		}

		else if (c == 0x15)
		{
			while (buf_len > 0)
			{
				back_up();
			}

			/* Non-editing character so insert it */
		}

		else
		{
			if (buf_len == (BUFLEN - 1))
			{
				usart_send_blocking(USART1, '\a');
			}

			else
			{
				buf[end_ndx] = c;
				end_ndx = inc_ndx(end_ndx);
				usart_send_blocking(USART1, c);
			}
		}
	}
}

/*
 * Called by libc stdio fwrite functions
 */
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

/*
 * Called by the libc stdio fread fucntions
 *
 * Implements a buffered read with line editing.
 */
int _read(int fd, char *ptr, int len)
{
	int my_len;

	if (fd > 2)
	{
		return -1;
	}

	get_buffered_line();
	my_len = 0;

	while ((buf_len > 0) && (len > 0))
	{
		*ptr++ = buf[start_ndx];
		start_ndx = inc_ndx(start_ndx);
		my_len++;
		len--;
	}

	return my_len; /* return the length we got */
}
