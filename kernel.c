#include <stddef.h>
#include <stdint.h>

#include "uart.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	uint8_t mem[256];
	uint8_t j = 0;

	uart_init();
	uart_puts("DiOS version 0.0.1 (pre-alpha)\n");

	while (1) {
		char buffer[256];

		uart_putc('>');

		uart_ngets(255, buffer);

		uint8_t i = 0;
		while (buffer[i] != 0) {
			switch (buffer[i]) {
			case '+':
				mem[j]++;
				i++;
				break;
			case '-':
				mem[j]--;
				i++;
				break;
			case '>':
				j++;
				i++;
				break;
			case '<':
				j--;
				i++;
				break;
			case '.':
				uart_putc(mem[j]);
				i++;
				break;
			case ',':
				mem[j] = uart_getc();
				i++;
				break;
			case ']':
				if (mem[j] == 0) {
					i++;
					break;
				}
				i--;
				uint8_t br = 1;
				while (1) {
					if (buffer[i] == '[') {
						if (--br == 0) break;
					} else if (buffer[i] == ']') {
						br++;
					}

					i--;
				}
				break;
			default:
				i++;
			}
		}
	}
}
