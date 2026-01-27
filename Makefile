ARCH=arm-none-eabi
OBJCOPY=$(ARCH)-objcopy
GCC=$(ARCH)-gcc

run: all
	qemu-system-arm -m 1G -M raspi2b -serial mon:stdio -kernel myos.elf -nographic

all: myos.elf kernel7.img

kernel7.img: myos.elf
	$(OBJCOPY) myos.elf -O binary kernel7.img

myos.elf: linker.ld boot.o kernel.o
	$(GCC) -T linker.ld -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel.o: kernel.c
	$(GCC) -mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -c kernel.c -o kernel.o -O2 -Wall -Wextra

boot.o: boot.S
	$(GCC) -mcpu=cortex-a7 -fpic -ffreestanding -c boot.S -o boot.o

clean:
	rm -f *.o myos.elf kernel7.img
