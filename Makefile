BUILD = build/

GCCPREFIX = arm-none-eabi-

GCCFLAGS = -c -mcpu=arm1176jzf-s -fno-builtin -fno-exceptions -fno-rtti -std=gnu++0x -O2 -Wall -Wextra

LDFLAGS = --nostdlib --no-undefined -O2

OBJECTS = $(addprefix $(BUILD),boot.o main.o graphic.o font.o logo.o console.o)

.PHONY: all
all: $(OBJECTS) kernel.elf kernel.img

$(BUILD)boot.o: boot.S
	$(GCCPREFIX)as $< -o $@

$(BUILD)main.o: main.cpp graphic.h console.h
	$(GCCPREFIX)g++ $(GCCFLAGS) $< -o $@

$(BUILD)graphic.o: graphic.cpp graphic.h common.h
	$(GCCPREFIX)g++ $(GCCFLAGS) $< -o $@

$(BUILD)font.o: font.cpp
	$(GCCPREFIX)g++ $(GCCFLAGS) $< -o $@

$(BUILD)logo.o: logo.cpp
	$(GCCPREFIX)g++ $(GCCFLAGS) $< -o $@

$(BUILD)console.o: console.cpp console.h graphic.h
	$(GCCPREFIX)g++ $(GCCFLAGS) $< -o $@

kernel.elf: $(OBJECTS) kernel.ld
	$(GCCPREFIX)ld $(LDFLAGS) $(OBJECTS) -T kernel.ld -o kernel.elf

kernel.img: kernel.elf
	$(GCCPREFIX)objcopy kernel.elf -O binary kernel.img

.PHONY: clean
clean:
	-rm kernel.img kernel.elf $(BUILD)*
