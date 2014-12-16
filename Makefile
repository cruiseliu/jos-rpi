# Makefile - build script */
# Copyright (C) 2013 Goswin von Brederlow <goswin-v-b@web.de>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# build environment
GCCPREFIX   =  arm-none-eabi-
BUILD       =  build/

# source files
SOURCES_ASM := $(wildcard *.S)
SOURCES_CC  := $(wildcard *.cpp)

# object files
OBJS        := $(patsubst %.S,%.o,$(SOURCES_ASM))
OBJS        += $(patsubst %.cpp,%.o,$(SOURCES_CC))
OBJS        := $(addprefix $(BUILD),$(OBJS))

# Build flags
DEPENDFLAGS := -MD -MP
BASEFLAGS   := -O2 -fpic -pedantic -pedantic-errors -nostdlib
BASEFLAGS   += -nostartfiles -ffreestanding -nodefaultlibs
BASEFLAGS   += -fno-builtin -fomit-frame-pointer -mcpu=arm1176jzf-s
WARNFLAGS   := -Wall -Wextra
ASFLAGS     := $(DEPENDFLAGS) -D__ASSEMBLY__
CXXFLAGS    := $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)
CXXFLAGS    += -fno-exceptions -std=c++0x

# build rules
all: kernel.img

include $(wildcard *.d)

kernel.elf: $(OBJS) kernel.ld
	$(GCCPREFIX)ld $(OBJS) -T kernel.ld -o $@

kernel.img: kernel.elf
	$(GCCPREFIX)objcopy kernel.elf -O binary kernel.img

clean:
	$(RM) -f $(OBJS) kernel.elf kernel.img

dist-clean: clean
	find -name "*~" -delete
	find -name "*.d" -delete

# C++.
$(BUILD)%.o: %.cpp Makefile
	$(GCCPREFIX)g++ $(CXXFLAGS) -c $< -o $@

# AS.
$(BUILD)%.o: %.S Makefile
	$(GCCPREFIX)g++ $(ASFLAGS) -c $< -o $@
