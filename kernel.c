#include <stddef.h>
#include <stdint.h>

static uint32_t MMIO_BASE;

static inline void mmio_init()
{
	MMIO_BASE=0x3f000000;
}

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t*)(MMIO_BASE + reg) = data;
}

static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t*)(MMIO_BASE + reg);
}

static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
			: "=r"(count): [count]"0"(count) : "cc");
}

enum
{
    // The offsets for reach register.
    GPIO_BASE = 0x200000,

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),

    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = (GPIO_BASE + 0x1000), // for raspi4 0xFE201000, raspi2 & 3 0x3F201000, and 0x20201000 for raspi1

    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),

    // The offsets for Mailbox registers
    MBOX_BASE    = 0xB880,
    MBOX_READ    = (MBOX_BASE + 0x00),
    MBOX_STATUS  = (MBOX_BASE + 0x18),
    MBOX_WRITE   = (MBOX_BASE + 0x20)
};

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

void uart_ngets(uint8_t n, char *buffer)
{
	char c;
	uint8_t i;
	for (i = 0; i != n; i++) {
		c = uart_getc();

		if (c == '\r') break;

		if (c == '\b') {
			i--;
			continue;
		}

		buffer[i] = c;
		uart_putc(c);
	}

	buffer[i] = 0;
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	uint8_t mem[256];
	uint8_t *ptr = mem;

	uart_init();
	uart_puts("the best os ever made.\n");

	while (1) {
		char buffer[256];

		uart_putc(uart_getc());

		for (uint8_t i = 0; buffer[i] != 0; i++) {
			switch (buffer[i]) {
			case '+':
				*ptr++;
				break;
			case '-':
				*ptr--;
				break;
			case '>':
				ptr++;
				break;
			case '<':
				ptr--;
				break;
			case '.':
				uart_putc(*ptr);
				break;
			case ',':
				*ptr = uart_getc();
				break;
			case ']': {
					  uint8_t br = 1;

					  if (*ptr == 0) break;

					  while (br != 0) {
						  if (buffer[i] == '[') br--;
						  else if (buffer[i] == ']')
							  br++;

						  i--;
					  }
				  }
				break;
			}
		}
	}
}
