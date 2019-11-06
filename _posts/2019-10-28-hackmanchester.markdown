---
layout: post
title:  "Hack Manchester挑战：固件基础设置"
date:   2019-10-28 15:23:44 +0000
categories: [HW, SW]
hidden: true
---
这个周末，Three Bodies参加了一个[Hack Manchester](https://www.hac100.com/event/HM2019/)的hackathon；25个小时为了弄
好任何有趣的项目，每个编程语言、每个平台都行。我们已经决定试一试把软件与硬件结合在一起；因此，决定了参加[ClockWork公司发的挑战](https://www.clockworksms.com/blog/hack-manchester-2019/)。简单来说，ClockWork挑战是“利用ClockWork短信API以构建一个帮助患长期疾病的人。我们不太了解患长期疾病的人一般靠什么工具，一般面对什么风浪，天天遇到什么问题。我们来调查[NHS](https://www.england.nhs.uk/)部门发行的报告，发现了患病的人难以遵照药物的安排；不是因问他们不要遵照安排，而是因为他们常常忘记、吃药物的安排没被描述清楚、效率低、副作用太严重了等等。所以，我们有意了解释下所示的问题：

* 帮病人想起来适时吃药物 （利用普通药盒子，最重要的是不管硬件出任何问题，病人还必须吃上他们的药物），
* 为了让病人汇报药物的效率、副作用提供一个最简单的方法（这个系统需要减少医生的压力），
* 给医生提供一个简单网上系统为了关照他们的“客户”情况（与其给病人打电话、发一封信，不如用更简单、更效率的方法），
* 最后，假如出任何问题（比如说病人意外地吃下错药物、忘记吃或者遇到一些副作用），给用户提供一个好用方法以和医生联系。

自然，我们设计的系统（**还😉**）并不是完全可靠的，我们只想证明这种系统是能建构的，而我们想探索一些新部件、新编程语言和平台。

# 固件设计
当然，还需要构建MCU固件。首先我们需要决定我们写的固件是要哪一个框架的，现代许多人使用Arduino框架。虽然Arduino现在丰富大数开源软件库、扩展，但对与习惯于“大电脑、大处理器”开发者来说，Arduino框架看似老旧DOS操作系统。（我认为是因为Arduino框架支持个个MCU平台、个个处理器，从而它只包括最基础的功能。）假如没有ESP32这种大功率的MCU，Arduino确实很合适。然而，为了利用所有ESP32提供的功能，我们最好使用[ESP-IDF](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/index.html)框架，特别是因为ESP-IDF支持FreeRTOS，开源实时操作系统。因此，我们可以使用我们的从“大电脑”习惯的软件开发方式。简单来说，我们该将应用分开成独立模块，然后靠消息队列把这些模块链接在一起。最重要的是，我们写的**每一个**模块中的代码都是一般无限循环（请注意，除了IRQ处理的函数以外，这样靠无限循的模块于Arduino都不能执行，因为Arduino只有一个循环，我们需要在这个循环办理一切。），FreeRTOS会将模块中的无限循环**抢占式调度**在合适处理器核心上。

看[ESP-IDF 快速入门](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/get-started/index.html)看似复杂，不过好在有另一个办法：PlatformIO。如下两个源码所示，为了设置一个用ESP-IDF的PlatformIO项目只需要进行一个命令，编辑一个文件。

{% highlight shell %}
$ platformio init
{% endhighlight %}

{% highlight ini linenos %}
# 保存在$PROJECT_ROOT/platform.ini
[common]
lib_deps =
    
[env:esp32]
platform = espressif32
board = esp32dev
framework = espidf
debug_tool = olimex-arm-usb-ocd-h
upload_port = /dev/cu.SLAB_USBtoUART
monitor_port = /dev/cu.SLAB_USBtoUART
monitor_speed = 115200
upload_speed = 230400
lib_deps =
    ${common.lib_deps}
build_unflags = -std=gnu++11
build_flags =
    -std=c++17

[env:native]
platform = native
targets = test
build_flags =
    -std=c++17
{% endhighlight %}

如上面的`platform.ini`所述，我们的项目包括两个环境：`esp32`和`native`。`esp32`环境描述怎么去编译ESP32代码，而`native`环境提供很方便的方法以测试平台无关的代码。开始实现项目以前，应该核实是否我们刚刚设置了PlatformIO会把项目编译好。请注意`esp32`环境里的`framework = espidf`（行8），其让PlatformIO使用ESP-IDF平台来编译代码，另外还要注意的参数是`build_flags`和`build_unflags`（行17、18、25），其两个参数所设定是编译器该使用哪一个C++标准，即[C++ 17](https://zh.cppreference.com/w/cpp/compiler_support#cpp17)，这样我们可以靠C++ 17的功能。既然大家都以`blink`为古典款Arduino的项目，我们也该来实现一个利用ESP-IDF/FreeRTOS的。当然`blink`并不需要任何C++ 17功能！

{% highlight C++ %}
// 保存在$PROJECT_ROOT/src/main.cpp

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"

#define BLINK_GPIO GPIO_NUM_5

extern "C" void app_main(void) {
  gpio_pad_select_gpio(BLINK_GPIO);
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
  while (1) {
    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
{% endhighlight %}

还剩下电路设计，从上面源码看来，电路也该是特别古典的：有一个ESP32（开发）版、一个电阻器、一个LED即可，如下面的电路图所示。随着`#define BLINK_GPIO GPIO_NUM_5`，请注意把电阻器接上`IO5`（设计如输出的）脚。

![古典电路](/assets/2019-10-28-hackmanchester/blink-sch.png){:class="img-responsive"}

硬件修好了后，到点该第一次来把项目编译、上传、执行。输入`platformio run -e esp32 -t upload`命令后，为了核实编译、上传过程都成功了，应该观察`platformio run`输出中的`success`，更重要的是该观察是否LED确实在闪烁。

{% highlight shell %}
$ platformio run -e esp32 -t upload
Processing esp32 (platform: espressif32; board: esp32dev; framework: espidf)
-----------------------------------------------------------------------------
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/espressif32/esp32dev.html
...
Use manually specified: /dev/cu.SLAB_USBtoUART
Uploading .pio/build/esp32/firmware.bin
esptool.py v2.6
Serial port /dev/cu.SLAB_USBtoUART
Connecting........___
Chip is ESP32D0WDQ5 (revision 1)
...
Writing at 0x00020000... (100 %)
Wrote 155920 bytes (78135 compressed) at 0x00010000 in 3.5 seconds...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...

Environment    Status    Duration
-------------  --------  ------------
esp32          SUCCESS   00:00:24.405
native         IGNORED
$
{% endhighlight %}

![古典blink在进行](/assets/2019-10-28-hackmanchester/blink-hw.jpeg){:class="img-responsive"}

利用PlatformIO以开发ESP-IDF固件最简单、最方便IDE一定是[Visual Studio Code](https://code.visualstudio.com/)；它包括PlatformIO插件，而它的C++插件也非常不错。安装这两个插件很简单：在Visual Studio Code，打开扩展，搜索C++和PlatformIO插件。

![Visual Studio Code插件](/assets/2019-10-28-hackmanchester/vscode0.png){:class="img-responsive"}

把插件安装好之后，`platformio run -e esp32 -t upload`命令也可以在Visual Studio Code来执行，打开PlatformIO扩展后执行`env:esp32 > Upload and Monitor`命令。

![Visual Studio Code PlatformIO命令](/assets/2019-10-28-hackmanchester/vscode1.png){:class="img-responsive"}

# 总结
这个帖子就结束这里。下一帖子我们会看如何接上一个控制LED的按钮，我们来探索两个方式为了处理按钮状态改变。
