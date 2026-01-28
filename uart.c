#include <stddef.h>

#include "uart.h"

uint32_t MMIO_BASE;

volatile unsigned int __attribute__((aligned(16))) mbox[9] = {
	9*4, 0, 0x38002, 12, 8, 2, 3000000, 0, 0
};

void uart_init()
{
	mmio_init();
	mmio_write(UART0_CR, 0x00000000);
	mmio_write(GPPUD, 0x00000000);
	delay(150);

	mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);

	mmio_write(GPPUDCLK0, 0x00000000);
	mmio_write(UART0_ICR, 0x7ff);

	mmio_write(UART0_IBRD, 1);
	mmio_write(UART0_FBRD, 40);

	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6)
			| (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));

	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(unsigned char c)
{
	while (mmio_read(UART0_FR) & (1 << 5)) {}
	mmio_write(UART0_DR, c);
}

unsigned char uart_getc()
{
	while (mmio_read(UART0_FR) & (1 << 4)) {}
	return mmio_read(UART0_DR);
}

void uart_puts(const char *str)
{
	for (size_t i = 0; str[i] != '\0'; i++)
		uart_putc((unsigned char) str[i]);
}

void uart_printnum(uint8_t n)
{
	uint8_t pt1, pt2;

	pt1 = (n & 0x0f);
	pt2 = (n & 0xf0) >> 4;

	if (pt1 > 9)
		pt1 += ('A' - 10);
	else
		pt1 += '0';


	if (pt2 > 9)
		pt2 += ('A' - 10);
	else
		pt2 += '0';

	uart_putc(pt2);
	uart_putc(pt1);
}

void uart_ngets(uint8_t n, char *buffer)
{
	char c;
	uint8_t i;
	for (i = 0; i != n; i++) {
		c = uart_getc();

		if (c == '\r') {
			uart_putc('\n');
			break;
		}

		if (c == '\b') {
			i--;
			uart_putc('\b');
			continue;
		}

		buffer[i] = c;
		uart_putc(c);
	}

	uart_puts("command received.\n");

	buffer[i] = 0;
}

