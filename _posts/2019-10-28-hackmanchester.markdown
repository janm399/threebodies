---
layout: post
title:  "Hack Manchester挑战：硬件"
date:   2019-10-28 15:23:44 +0000
categories: [Hardware]
hidden: true
---
这个周末，Three Bodies参加了一个[Hack Manchester](https://www.hac100.com/event/HM2019/)的hackathon；25个小时为了弄
好任何有趣的项目，每个编程语言、每个平台都行。我们已经决定试一试我们要把一般软件与硬件结合在一起；因此，
决定了参加[ClockWork挑战](https://www.clockworksms.com/blog/hack-manchester-2019/)。简单来说，ClockWork挑战是
“利用ClockWork短信API以构建一个帮助患长期疾病的人的app。我们不太了解患长期的疾病的人一般靠什么工具，
一般必须面对什么问题，天天一般遇到什么问题。我们来调查最新[NHS](https://www.england.nhs.uk/)消息，
发现了患病的人很好不容易是遵照药物的安排：病人常常错过适时吃药物（不是因问他们不要遵照安排，
而是因为他们常常忘记、药物的安排没被描述得清楚、药的副作用太严重了等等）。
所以，我们决定构建一个把硬件与软件结合在一起的系统。

![基础电路构建](/assets/2019-10-28-hackmanchester/b0.png){:class="img-responsive"}

TODO：更细节得描述电路构建，加上发光二极管、红外传感器、按钮、（高电流）电源；特涅是SIM800L的电压需求

![第一版](/assets/2019-10-28-hackmanchester/step0.jpeg){:class="img-responsive"}

![第二版](/assets/2019-10-28-hackmanchester/step1.jpeg){:class="img-responsive"}

当然，还需要给MCU跑固件：多亏ESP8266和ESP32的编译器是比较现代的C++标准，除了一些最新对象、算法
（比如`std::optional`、[structured bindings](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0217r3.html)、`std::optional`等等）以外，我们还可以靠许多C++ `std` namespace里的东西。TODO：干嘛？

所以，写固件的时候应该按照一般“clean code”的规则，尤其加上仔细的测试。不过，拿利用些MCU里的硬件的功能的代码来说，
在x86处理器进行下到底怎么来写细节测试代码？比如，我们靠`<SoftwareSerial>`来连接GSM配件，就算你的用x86处理器的
电脑有这么古老串行端口，写x86测试代码肯定会出”works on my machine“，可是并不在MCU跑的这种的问题。

{% highlight C++ %}
// 普通C++ stdlib内存机器，没问题
#include <memory> 
// 靠MCU里的的硬件机器之一，显然有问题
#include <SoftwareSerial.h>

class transport {
private:
    SoftwareSerial serial; // ！！！
    
public:
    void begin(); // 想象方法的实践利用serial字段
};
{% endhighlight %}

....