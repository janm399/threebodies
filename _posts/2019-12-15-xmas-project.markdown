---
layout: post
title:  "好用圣诞节项目"
date:   2019-12-14 18:00:00 +0000
categories: [HW, SW]
excerpt: "***"
---
本帖子所谈是特别好用圣诞节项目，至少在nerd看来特别好用项目。基本上项目的能力很简单：它是一个夜间小灯，很多人夜间中上厕所时不想开大灯（因为太亮了），该小灯的光不仅仅不太亮，而且光颜色适合夜间。因此，本项目的需求包括：

* 不许白天时开灯，
* 不许打扰昼夜节律，
* 必须逐渐地打开或关上，
* 必须自动打开，关上，
* 必须使用内部电源，必须支持最方便充电的方法

从上面的需求可见的项目至少需要（1）光传感器；以确切控制发光的颜色，最简单、最合适的是我们已经谈过（2）2812LED元件；以实现自动打开/关上的能力我们来依靠（3）人体红外传感器。内部电源会利用（4）锂电池，其最方便充电方法一定是无线的，因此最后一个元件是（5）一个无线充电器接收器。

因为本项目计算力的要求确实很低，与其他项目不同的是该项目会——为了减少功耗——使用另一个MCU，而且由于时间不够，以驱动2812我们会依靠Arduino框架。实际上，每一个项目都应该考虑如何把电力与计算力需求均势好。而且，如我们会探索的，实现项目时必须考虑如何得到最高固件的效率。这意味着千万避免忙碌循环，利用处理器或开发平台所有的节省资源的状态，仔细地考虑项目对MCU中的元件的确切需求。若我们选的处理器/MCU包括某些我们不需要的元件，我们最好找如何让它们进入休眠状态。

# 3D设计
项目不仅仅需要硬件和固件，而且还需要一个相应容器。该容器的设计必须提供足够内部空间以装下所有的元件，尤其无线充电接收器；外部必须提供相应空间为了安装2812。最终，该容器还需要让我们安装向外部“看着”的人体外红传感器。随着最大元件（无线充电接收器）的大小，内部尺寸是$$85mm$$乘$$40.5mm$$；人体外红传感器的容器是一个有$$25mm$$高、$$25mm$$直径的半球形结构，如下面图样所示。

![](/assets/2019-12-15-xmas-project/box.png)

该盒子有三个配件，盖子和内部盒子是使用一般PLA打印出的，外部的带子是使用透明PLA打印出的。下面视频所示是如何把其三个配件装配在一起。

<div class="myvideo">
   <video  style="display:block; width:100%; height:auto;" controls loop="false">
       <source src="{{ site.baseurl }}/assets/2019-12-15-xmas-project/box.mp4" type="video/mp4" />
   </video>
</div>


<script src="https://cdn.babylonjs.com/babylon.js"></script>
<script src="https://cdn.babylonjs.com/viewer/babylon.viewer.js"></script> 
<script src="https://cdn.babylonjs.com/loaders/babylonjs.loaders.min.js"></script>
<script src="https://cdn.babylonjs.com/loaders/babylon.stlFileLoader.js"></script>

跟其他项目不同，我们首先来建立一个3D模型；一种盒子

<babylon templates.main.params.fill-screen="true">
    <model url="{{ site.baseurl }}/assets/2019-12-15-xmas-project/box.stl">
        <material material-type="standard">
            <diffuse-color r='1' g='0' b='0'></diffuse-color>
            <ambient-color r='1' g='0' b='0'></ambient-color>
            <main-color r='0' g='0' b='0'></main-color>
        </material>
    </model>
    <scene>
        <main-color r='0' g='0' b='0'></main-color>
    </scene>
</babylon>

如果你想把模型打印出来，下载[容器3D结构STL]({{ site.baseurl }}/assets/2019-12-15-xmas-project/box.stl)和[容器图样PDF]({{ site.baseurl }}/assets/2019-12-15-xmas-project/box.pdf)。

# 硬件设计
一开始我们打算使用Arduino nano；虽然Arduino nano没有ESP32那么搞结算能力、那么多输入和输出脚，但是以控制一个2812、接收一个人体外红传感器输出、接收一个周围光亮度传感器输出；简单来说在输出和输入（一个数字输出、一个数字输入、一个模拟输入）上Arduino nano足够了。只不过，我们应该回想上面所提到的效率问题：即使本项目能够由Arduino nano实现的，我们应该再考虑是否有另外一个更合适MCU。原来有，即ATTiny组！

如下面表格所示，ESP32、Arduino nano的性能、接下来功耗太大了，尤其从本项目的固件需求看来。

|              | ESP32  | Arduino nano | ATtiny25 | ATtiny25 |
|--------------|--------|--------------|----------|----------|
| 处理器核     | 2      | 1            | 1        | 1        |
| 处理器频率   | 240MHz | 100MHz       | 8MHz     | 8MHz     |
| 内存         | 256kiB | 32kiB        | *8*kiB   | *2*kiB   |
| GPIO         | 32     | 10           | 4        | 4        |
| 中断         | 10     | 4            | 2        | 2        |
| 所需电压     | 3.3V   | 5V           | > 2.0V   | > 2.0V   |
| 所需电流     | <40mA  | < 22mA       | < 8mA    | < 8mA    |

配合本项目的计算能力的需求、GPIO数的需求、尽量减少功耗的需求与可获得MCU。一时想象可获得的MCU只包括上面的表格所示的：在此情况下，可见相应MCU是ATTiny组。以决定哪一个具体ATtiny MCU该选择，首先要实现固件…… 这样恶性循环：为了知道如何来实现固件我们首先需要选好一个MCU，为了选好一个MCU我们首先需要编程好固件。好在，ATTiny MCU组中唯一个区别是MCU里（flash和RAM）内存大小，其他特性都完全一样。因此，一开始可以随便地选择，现实固件后再证实我们本来选的MCU是不是最合适的。结果，下面图所示是电路初期版本。

![](/assets/2019-12-15-xmas-project/box-sch-v1.png)

唯一个元件上面电路图不所示是Qi无线充电接收器，该元件可以直接上到LiPO元件`IN+`、`IN-`的输入。为了简单化改修过程，最好在电池和LiPO元件的中间接上一个接头。

# 固件设计
最简单固件实现方法是在忙碌循环中测量人体外红传感器值；若有人再来测量光亮度传感器值；若二者传感器值都在合适范围其中，就逐渐开灯。开灯后，延时一定一段时间，接下来关灯。不过，问题来了：随着我们已经提供的减少功耗需求，我们最好避免忙碌循环，因为忙碌循环会阻止处理器进入某些低功耗状态（通常由减低处理器核的时钟源频率）。因此，与其依靠忙碌循环，不如利用GPIO中断。尤其中断会自动将处理器低功耗状态成为到高的。

我们一步一步来实现固件需要的代码吧。首先我们需要决定如何上传固件。跟一般Arduino不同的是我们不会直接来烧录固件，由于ATtiny不包含USB接口。我们需要使用一个烧录器，要么JTAG烧录器，要么另一个Arduino，比如Arduino nano。其JTAG烧录器确实不复杂，如下面的电路图所示。

![](/assets/2019-12-15-xmas-project/isp.png)

接下来只需要配置PlatformIO，如下面的`platform.ini`文件所示。

{% highlight ini %}
[common]
port=/dev/cu.wchusbserial330

[env:attiny]
framework = arduino
platform = atmelavr
board = attiny85
board_build.mcu = attiny85
board_build.f_cpu = 8000000L
lib_deps =
    FastLED
upload_protocol = stk500v1
upload_flags =
    -P${common.port}
    -b19200
    -F
upload_port = ${common.port}
upload_speed = 19200
{% endhighlight %}

最重要的是配置好ISP协议、ISP接口；你的设备一定不一样，特别是`port`，在Linux或者macos下，该上传的接口会随着`/dev/cu.wchusbserial####`，其中`####`是一个数字列。请注意，除了`port`和`upload_protocol`以外，该`platform.ini`需要定义处理器的时钟源的频率，即$$8MHz$$（更低频率会影响控制2812的框架，尤其影响我们能控制都少像素）。配置完Platform IO可以测试能否烧录最简单固件，按照Arduino框架，该最简单固件必须包含两个函数：`setup()`和`loop()`。Arduino框架会处理器启动时执行`setup()`函数，在一般情况下，该函数负责配置所有所需的接口，即接口的输入，输出状态。`setup()`执行完，Arduino框架会不断执行`loop()`函数。

{% highlight C++ %}
#include <Arduino.h>

void setup() {
}

void loop() {
}
{% endhighlight %}

写（复制&粘贴）如上面的代码后可以证实能否由Arduino nano ISP来烧录固件。查看进行`pio run -e attiny -t upload`命令的输出可见是否烧录过程成功了。

{% highlight shell %}
$ pio run -e attiny -t upload 
Processing attiny (framework: arduino; platform: atmelavr; board: attiny85)
...
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
DATA:    [          ]   4.3% (used 22 bytes from 512 bytes)
PROGRAM: [          ]   4.4% (used 360 bytes from 8192 bytes)
Configuring upload protocol...
AVAILABLE: stk500v1
CURRENT: upload_protocol = stk500v1
Looking for upload port...
Uploading .pio/build/attiny/firmware.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.08s
...
Writing | ################################################## | 100% 0.79s
...
Reading | ################################################## | 100% 0.51s
...
=================== [SUCCESS] Took 4.16 seconds ===================
{% endhighlight %}

成功后，我们需要考虑如何安装中断办理的函数、如何读取外红人体传感器输出、如何读取范围光亮值、如何控制2812。一来要解决最麻烦的任务，即中断办理设计。与ESP32相反，ATtiny不仅仅只有一个管理GPIO的中断，而且实现ATtiny固件时无法利用Arduino框架提供的`attachInterrupt`函数。

# Demo
