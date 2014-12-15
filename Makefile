.PHONY: all
all: entry.o init.o graphic.o font.o logo.o kernel.elf kernel.img

entry.o: entry.S
	arm-none-eabi-as entry.S -o entry.o

init.o: init.cpp graphic.h
	arm-none-eabi-g++ -mcpu=arm1176jzf-s -fno-builtin -fno-exceptions -fno-rtti -std=gnu++0x -c init.cpp -o init.o -O2 -Wall -Wextra

graphic.o: graphic.cpp graphic.h common.h
	arm-none-eabi-g++ -mcpu=arm1176jzf-s -fno-builtin -fno-exceptions -fno-rtti -std=gnu++0x -c graphic.cpp -o graphic.o -O2 -Wall -Wextra

font.o: font.cpp
	arm-none-eabi-g++ -mcpu=arm1176jzf-s -fno-builtin -fno-exceptions -fno-rtti -std=gnu++0x -c font.cpp -o font.o -O2 -Wall -Wextra

logo.o: logo.cpp
	arm-none-eabi-g++ -mcpu=arm1176jzf-s -fno-builtin -fno-exceptions -fno-rtti -std=gnu++0x -c logo.cpp -o logo.o -O2 -Wall -Wextra

kernel.elf: entry.o init.o graphic.o font.o logo.o
	arm-none-eabi-ld -T kernel.ld -o kernel.elf -O2 --no-undefined --nostdlib entry.o init.o graphic.o font.o logo.o

kernel.img: kernel.elf
	arm-none-eabi-objcopy kernel.elf -O binary kernel.img

.PHONY: clean
clean:
	rm *.o kernel.elf kernel.img
