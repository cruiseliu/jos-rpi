## 源文件列表

文件名                          | 主要功能
--------------------------------|--------------------------
boot.S                          | 启动文件, 初始化栈和 bss
main.cpp                        | C 代码入口, 主要负责初始化
uart.h <BR> uart.cpp            | UART I/O, 来自 raspbootin
screen.h <BR> screen.cpp        | Framebuffer 操作
font.cpp                        | Screen 使用的字体数据
logo.cpp                        | Screen 使用的logo数据
console.h <BR> console.cpp      | I/O 设备的封装
monitor.h <BR> monitor.cpp      | 内核 shell
kdebug.h <BR> kdebug.cpp        | 获取函数名和行号等调试信息
stdio.h <BR> stdio.cpp          | 将 Console 封装成标准库格式
string.h <BR> string.cpp        | 类似标准库
addr.h                          | 地址/指针类型
arm.h                           | 内联 ARM 指令
lib/\_*div*.S                   | 除法库, 来自 Chromium OS
lib/libcsud.a<BR>lib/keyboard.S | 键盘驱动, 来自剑桥教程

其他文件中除了 kernel.ld 和 Makefile 外都是非必须的.

其中 boot.S, main.cpp, uart.h/cpp, kernel.ld 这几个文件和之前提交过的报告中差别不大, 不再赘述.

以下是其他硬件相关部分的说明.

## Framebuffer

JOS 中的输出有串口, 并口, VGA text-mode 这三种方式, 其中串口和并口直接输出字节流, 和 UART 类似, VGA text-mode 则是 PC 特有的功能, 树莓派及其他 ARM 平台均无法使用.

树莓派中 CPU 和 GPU 使用"邮箱"互相通信. Framebuffer 的初始化方式是 CPU 在内存中分配一段空间并写入配置, 然后将其地址通过邮箱发给 GPU, GPU 收到后会将包括 framebuffer 指针在内的一些必要信息写回这个地址, 之后 CPU 就可以直接向 framebuffer 读取或写入每个像素的颜色.

方便起见我使用了32位色深并禁用了 alpha 通道, 这种设置的效果和24位色是一样的, 但因为 framebuffer 中每个像素的地址都是按字对齐的所以操作更加方便.

为了显示文字, 我们还需要一个点阵字体(当然如果你够闲用 TrueType 也不是不行), 我在网上找了一个 [Proggy Clean](http://www.proggyfonts.net/download) 字体, 然后半手动地把它转成了二进制位图格式.

另外为了展示高端大气的 framebuffer 模式之优越性, 我在启动屏幕上加了一个北大的 logo, 没什么实际意义.

现存的主要问题是滚屏速度太慢, 我也不知道该怎么办.

这部分内容主要参考了 [eLinux 文档](http://elinux.org/RPi_Framebuffer) 和 [剑桥的教程](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/).

相关文档: Screen

## 除法例程

树莓派使用的是 ARMv6 处理器, 这种架构的一大特色是没有除法指令.

从编译器的报错可以看出来, gcc 使用的除法函数是 `__aeabi_uldivmod` 等, 把这个名字扔到 Google 上搜一下就能找到 Chromium OS 的[源代码](https://chromium.googlesource.com/chromiumos/third_party/u-boot-v1/+/master/arch/arm/lib/_uldivmod.S), 我直接复制过来用了, 只需要改一点点.

## Backtrace

首先要加上 `-fno-omit-frame-pointer` 编译参数, 直接用栈指针和机器码猜栈帧大小我可玩不来.

ARM 的栈帧结构在官方文档中有[介绍](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0041c/Cegbajdj.html), <EM>不过是错的</EM>

反汇编可以看到真正的栈帧是这样生成的:

    push {fp, lr}
    add  fp, sp, #4

所以 fp 指向的是 lr (旧 pc) 没错, 但它下面的就是旧 fp, 再下面就是通用寄存器了. 知道这些 backtrace 就好写了.

另外在网上看到了个读寄存器的简易方法:

    register uint32_t reg_fp asm("fp");
    uint32_t fp = reg_fp;

个人觉得比内联汇编优雅.

相关文档: Monitor::backtrace

## 其他

键盘驱动直接用了别人的库, 个人认为现在还不是纠结 USB 的时机.

因为用的是 framebuffer 所以彩色输出就是非常自然的功能了, UART 那边 bash 本来就支持彩色输出所以不需要我们操心.

至此 Lab 1 所有功能都已经完成, 现在这个系统既可以只用一根刷机线连到电脑上在教室里跑, 也可以不要电脑直接插键盘显示器用, 带行号的 backtrace 和彩色输出等比较高级的功能该有的也都有, 我自认为已经是非常完善了.

Lab 2 恐怕没时间做了...
