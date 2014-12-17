JOS-RPi
=======

Port of MIT JOS to Raspberry Pi.

Usage
-----

Use `mkdir -p build/lib && make` to build.

You will find `kernel.elf` and `kernel.img` in project directory.

If you have a Raspberry Pi with RASPBIAN, copy `kernel.img` to the boot partition and connect your computer to it's UART port.
You can use [raspbootin](https://github.com/mrvn/raspbootin) to simplify this progress.

If you prefer an emulator such as [QEMU](https://github.com/Torlus/qemu/tree/rpi), just load `kernel.elf`.

Roadmap
-------

  * Lab 1
    * ~~Bootloader~~
    * ~~Framebuffer~~
    * ~~Text ouput~~
    * ~~UART I/O~~
    * ~~Console~~
    * ~~Fromatted strings~~
    * ~~Kernel shell~~
    * ~~Backtrace~~
    * Symbol table
    * Colorful output

  * Lab 2
    * ......

TODO
----

  * Put source files into subdirectories
  * Generate documantation
  * Implement a USB keyboard driver

References
----------

 1. [MIT JOS](http://pdosnew.csail.mit.edu/6.828/2014/)
 2. [OSDev tutorial](http://wiki.osdev.org/ARM_RaspberryPi_Tutorial_C) (UART I/O)
 3. [Cambridge tutorial](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/) (assembly)
 4. [eLinux RPi framebuffer article](http://elinux.org/RPi_Framebuffer)
 5. [Proggy font](http://www.proggyfonts.net/download/)
 6. [raspbootin](https://github.com/mrvn/raspbootin)
