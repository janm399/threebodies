---
layout: post
title:  "Hack Manchester挑战"
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

自然，我们设计的系统（**还**）并不是完全可靠的，我们只想证明这种系统是能建构的，而我们想探索一些新部件、新编程语言和平台。

## 固件设计
当然，还需要构建MCU固件。首先我们需要决定我们写的固件是要哪一个框架的，现代许多人使用Arduino框架。虽然Arduino现在丰富大数开源软件库、扩展，但对与习惯于“大电脑、大处理器”开发者来说，Arduino框架看似老旧DOS操作系统。（我认为是因为Arduino框架支持个个MCU平台、个个处理器，从而它只包括最基础功能。）假如没有ESP32这种大功率的MCU，Arduino确实很合适。然而，为了利用所有ESP32提供的功能，我们最好使用[ESP-IDF](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/index.html)框架，特别是因为ESP-IDF支持FreeRTOS，开源实时操作系统。因此，我们可以使用我们的从“大电脑”习惯的软件开发方式。简单来说，我们该将应用分开成独立模块，然后靠消息队列把这些模块链接在一起。最重要的是，我们写的**每一个**模块中的代码都是一般无限循环（请注意，除了IRQ处理的函数以外，这样靠无限循的模块于Arduino都不能执行，因为Arduino只有一个循环，我们需要在这个循环办理一切。）

看[ESP-IDF 快速入门](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/get-started/index.html)看似很复杂，不过好在有另一个办法：PlatformIO。如下两个源码所示，为了设置一个用ESP-IDF的PlatformIO项目只需要进行一个命令，编辑一个文件。

{% highlight shell %}
$ platformio init
{% endhighlight %}

{% highlight ini linenos %} 
// 保存在$PROJECT_ROOT/platform.ini
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
lib_extra_dirs = src
test_build_project_src = true
build_flags =
    -std=c++17
    -I test_include
{% endhighlight %}

如上`platform.ini`所述，我们的项目包括两个环境：`esp32`和`native`。`esp32`环境一方面描述怎么来编译ESP32的代码，`native`环境一方面提供很方便方法来测试平台无关的代码。我没开始实现我们的项目以前，应该首先查看是否我们刚刚设置PlatformIO会把项目编译好。请注意`esp32`环境里的`framework = espidf`（行8），其让PlatformIO使用ESP-IDF平台来编译代码，另外还要注意的参数是`build_flags`和`build_unflags`（行17、18、26），其两个参数所设定是编译器该使用哪一个C++标准，即[C++ 17](https://zh.cppreference.com/w/cpp/compiler_support#cpp17)，这样我们可以靠C++ 17的功能。既然大家都以`blink`为古典款Arduino的项目，我们也该来实现一个利用ESP-IDF/FreeRTOS的。当然`blink`并不需要任何C++ 17功能！

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

TODO: 在[Visual Studio Code]()，打开PlatformIO扩展后执行`env:esp32 > Upload and Monitor`命令。

# TODO
如何从blink到目的，如何来实现FreeRTOS的模块；再说，删除这个Arduino（测试）代码。

VVV

## 电路设计
![基础电路构建](/assets/2019-10-28-hackmanchester/b0.png){:class="img-responsive"}
*电路设计*

这个电路包括五个独立配件：
* (1) 项目核心是8266MCU部件—虽然它没有足够输出，但是还合适这个比较简单的hack，
* (2) 为了把项目连接上GSM网络，我们使用SIM800L GSM部件，与大家都习惯的相反，它使用“古代”串行接口，
* (3), (3ʹ) 每一个药盒子都包括三个LED，两个发看得见光的，一个发红外光的；另外，每一个药盒子有感红外的传感器，
* (4) 为了表示通知，我们用最简单方法：LED，
* (5) 最后，我们需要加上三个按钮，可是8266没有足够输入脚，只剩下一个模拟输入；因此，我们不得不需要加上电压分配，然后利用MCU中的A0脚（我们承认是个该死的hack）。

再说，另一个重点是SIM800L部件需要3.7-4.1V电源，从8266可以获得的3.3V电源是完全无法利用的。一开始我们决定了利用这个3.3V电源，SIM800L好像没有问题，可是接不上GSM服务。我们在把它调试的时候，我们注意到SIM800L纷纷发出`SMS Ready`，接下来`Call Ready`，然后（我们估计为了电源电压太低）被重启了。注意，SIM800L最大电流需求上2A，从而，我们不得不加上了一个高电流的电压稳压器[MIC29150](http://www.farnell.com/datasheets/94451.pdf)。

大遗憾是ESP32不包括builtin-LED，所以为了查看

上面代码所示是一个跟Arduino古典`blink`一样的概念，唯一区别是ESP-IDF没有Arduino中的两个函数`setup`和`loop`，ESP-IDF只有一个函数需要实现，想象这个函数是Linux中的`init`编程。Linux核型启动的时候，它也执行`init`编程，然后`init`负责处理剩下的启动过程。

多亏[PlatformIO]()中ESP32工具链包括现代C++编译器（是编译器是支持C++17的！），我们很容易就可以利用现代C++标准的方法。使用ESP32

所以，写固件的时候应该按照所有“clean code”的规则，尤其加上仔细的测试。不过，拿利用些硬件的功能来说，在x86处理器进行下到底怎么来写细节的测试代码？比如，我们靠`SoftwareSerial`来连接GSM配件，就算你的用x86_64处理器的电脑有这么古老串行端口。你写好x86_64测试代码之后把它进行在这么快处理器跑肯定会出”works on my machine“，可是并不在MCU这样的问题。我们可以下载一些虚拟硬件的库，不幸的是这些库一般指支持根本Arduino固件，为了来测试更复杂接口、更复杂MCU能力，我们不得不需要自己写这种虚拟硬件库。

从我们的[GitHub版本库](https://github.com/teroxik/hackmanchester2019-hw)举一个例子：怎么虚拟`SoftwareSerial`，然后怎么来使用这个虚拟`SoftwareSerial`？

{% highlight C++ %}
class SoftwareSerial {
 public:
  void println(char *);
  //...
}
{% endhighlight %}

那么，`SoftwareSerial`是在Arduino固件包括的，x86_64并不包括Arduino固件；当然，可以很容易地自己写一个x86_64实现，比如：

{% highlight C++ %}
using namespace std;
class SoftwareSerial {
 private:
  vector<string> writes;

 public:
  void println(char *line) { writes.push_back(string(line)); }
}
{% endhighlight %}

到目前为止一切顺利吧，不过怎么加上测试中的expectations，固件代码不类似于利用dependency injection的代码。我们代码一般直接地构建`SoftwareSerial`对象。好在使用对象`SoftwareSerial`靠它的`begin(...)`方法；从而，我们虚拟实现只需要提供合适地实现`begin(...)`方法。

{% highlight C++ %}
class SoftwareSerial {
 public:
  void begin(int baud, int rx_pin, int tx_pin) {
    software_serial_mock::instance().add(this, baud, rx_pin, tx_pin);
  }
}
{% endhighlight %}

如下源码所示，`software_serial_mock`是控制虚拟`SoftwareSerial`的接口，每次我们的固件调用`SoftwareSerial.begin(...)`，我们把刚刚调用`SoftwareSerial`对象放在`software_serial_mock`里。最简单来说，`software_serial_mock`是一种版本库，我们写测试代码一般会调用这个版本库中`get`方法为了获取`SoftwareSerial`对象。请注意，在production环境下，`get`方法应该是返回`optional`值；不过这里`get`只是返回`SoftwareSerial`指针或者--当找不到匹配的`SoftwareSerial`--`nullptr`的。

{% highlight C++ %}
using namespace std;
class software_serial_mock {
 private:
   vector<tuple<int, int, int, SoftwareSerial *>> mock_instances;

 public:
  void add(SoftwareSerial *instance, int baud, int rx_pin, int tx_pin) {
    mock_instances.emplace_back(make_tuple(baud, rx_pin, tx_pin, instance));
  }
  SoftwareSerial *get(const int rx_pin) const;
};
{% endhighlight %}

XXX

{% highlight C++ %}
void test_xxx() {
  transport t;
  t.loop();

  auto writes = software_serial_mock::instance().get(pins::sms_rx)->get_writes();
  // 调查writes里有没有对的质量
}

extern "C" int main(int, char **) {
  UNITY_BEGIN();
  RUN_TEST(test_xxx);
  UNITY_END();
  return 0;
}
{% endhighlight %}

显然，调查`get_writes()`，返回`std::vector<std::string>`的方法很麻烦；更方便的是加上一种控制接口；把它实现在`SoftwareSerial`与`software_serial_mock`，如下例所示：

{% highlight C++ %}
using namespace std;
class software_serial_control {
 public:
  virtual void rx(string rx, 
                  chrono::duration<uint, milli> timeout) = 0;
  virtual void tx(string tx) = 0;
  virtual void add_auto_rxtx(string request, 
                             string response,
                             chrono::duration<uint, milli> timeout) = 0;
  virtual void set_blocking(bool blocking) = 0;
};
{% endhighlight %}
