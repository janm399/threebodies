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

因为本项目计算力的要求确实很低，与其他项目不同的是该项目会——为了节省电池提供的电量——使用另一个MCU，而且由于时间不够，以驱动2812我们会依靠Arduino框架。实际上，每一个项目都应该考虑如何把电力与计算力需求均势好。而且，如我们会探索的，实现项目时必须考虑如何得到最高固件的效率。这意味着千万避免忙碌循环，利用处理器或开发平台所有的节省资源的状态，仔细地考虑项目对MCU中的元件的确切需求。若我们选的处理器/MCU包括某些我们不需要的元件，我们最好找如何让它们进入休眠状态。

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
一开始我们像使用Arduino nano

# 固件设计
这意味着千万避免忙碌循环（这个阻止处理器进入某些节省电的状态，通常由减低处理器核的时钟源频率。）

# Demo