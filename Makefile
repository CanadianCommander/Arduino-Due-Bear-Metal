OBYCP = ../tools/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-objcopy
CC = arm-none-eabi-gcc
CFLAGS = -O0 -mcpu=cortex-m3 -mthumb
INCLD = -I ../tools/ATMEL/ -I ../tools/CMSIS/include

test.bin : test.elf
	$(OBYCP) -O binary test.elf test.bin

test.elf : test.o
	$(CC) $(CFLAGS) -nostdlib -ffreestanding -Wl,--entry=start -march=armv7-m -Tlinker-script.ld test.o -o test.elf

test.o : test.c
	$(CC) $(CFLAGS) -c -o test.o test.c $(INCLD)

clean:
	rm test.o test.elf test.bin

reset:
	stty -f /dev/cu.usbmodem14111 1200
	sleep 1
	stty -f /dev/cu.usbmodem14111 hupcl
	sleep 1

upload : test.bin reset
	bossac -i -d --port=cu.usbmodem14111 -U=false -e -w -v -b test.bin
	stty -f /dev/cu.usbmodem14111 9600 # restar board
