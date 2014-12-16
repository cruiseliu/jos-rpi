JOS-RPi
=======

Port of MIT JOS to Raspberry Pi.

Usage
-----

Use `mkdir build && make` to build.

You will get `kernel.elf` and `kernel.img` in project directory.

If you have a Raspberry Pi with RASPBIAN, copy `kernel.img` to the boot partition.

If you prefer an emulator such as [QEMU](https://github.com/Torlus/qemu/tree/rpi), just load `kernel.elf`.

Roadmap
-------

  * ~~Bootloader~~
  * ~~Framebuffer~~
  * ~~Text ouput~~
  * ~~Console output~~
  * Keyboard input
  * Console input
  * Fromatted strings
  * Kernel shell
  * Backtrace
  * ......

TODO
----

  * Write a better makefile
  * Put source files into subdirectories
  * Set up a [serial bootloader](http://wiki.osdev.org/ARM_RaspberryPi#Boot-from-serial_kernel)
  * Generate documantation

References
----------

 1. [MIT JOS](http://pdosnew.csail.mit.edu/6.828/2014/)
 2. [OSDev tutorial](http://wiki.osdev.org/ARM_RaspberryPi_Tutorial_C) (UART I/O)
 3. [Cambridge tutorial](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/) (assembly)
 4. [eLinux RPi framebuffer article](http://elinux.org/RPi_Framebuffer)
 5. [Proggy font](http://www.proggyfonts.net/download/)
