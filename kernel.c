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

		uart_putc('$');

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
			case '|':
				for (uint8_t k = 0; k < 255; k++)
					mem[k] = 0;
				i++;
				break;
			case 'p':
				/* thank me later */
				uart_printnum(mem[j]);
				uart_putc('\n');
				i++;
				break;
			case 'd':
				for (uint8_t k = 0; k < 0xff; k++) {
					uart_printnum(mem[k]);
					if (((k+1) & 0x0f) == 0x00) {
						uart_putc('\n');
					} else {
						uart_putc(' ');
					}
				}

				uart_printnum(mem[0xff]);
				uart_putc('\n');
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
