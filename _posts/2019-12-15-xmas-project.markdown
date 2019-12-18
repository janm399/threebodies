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

配合本项目的计算能力的需求、GPIO数的需求、尽量减少功耗的需求与可获得MCU。一时想象可获得的MCU只包括上面的表格所示的：在此情况下，可见相应MCU是ATTiny组。以决定哪一个具体ATtiny MCU该选择，首先要实现固件…… 这样恶性循环：为了知道如何来实现固件我们首先需要选好一个MCU，为了选好一个MCU我们首先需要编程好固件。好在，ATTiny MCU组中唯一个区别是MCU里（flash和RAM）内存大小，其他特性都完全一样。因此，一开始可以随便地选择，现实固件后再证实我们本来选的MCU是不是最合适的。

![](/assets/2019-12-15-xmas-project/box-sch.png)

唯一个元件上面电路图不所示是Qi无线充电接收器，该元件可以直接上到LiPO元件`IN+`、`IN-`的输入。

# 固件设计
最简单固件实现方法是在忙碌循环中测量人体外红传感器值；若有人再来测量光亮度传感器值；若二者传感器值在相应的范围其中，就逐渐开灯。开灯后，延时一定一段时间，接下来关灯。不过，问题来了：随着我们已经提供的减少功耗需求，我们最好避免忙碌循环，因为忙碌循环会阻止处理器进入某些低功耗状态（通常由减低处理器核的时钟源频率）。因此，与其依靠忙碌循环，不如利用GPIO中断。尤其中断会自动将处理器低功耗状态成为到高的。

# Demo
