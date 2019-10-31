---
layout: post
title:  "Hack Manchester挑战：硬件"
date:   2019-10-28 15:23:44 +0000
categories: [Hardware]
hidden: true
---
这个周末，Three Bodies参加了一个[Hack Manchester](https://www.hac100.com/event/HM2019/)的hackathon；25个小时为了弄
好任何有趣的项目，每个编程语言、每个平台都行。我们已经决定试一试把软件与硬件结合在一起；因此，决定了参加[ClockWork挑战](https://www.clockworksms.com/blog/hack-manchester-2019/)公司发的挑战。简单来说，ClockWork挑战是“利用ClockWork短信API以构建一个帮助患长期疾病的人。我们不太了解患长期疾病的人一般靠什么工具，一般面对什么问题，天天遇到什么问题。我们来调查[NHS](https://www.england.nhs.uk/)部门发行的报告，发现了患病的人难以遵照药物的安排：病人常常错过适时吃药物（不是因问他们不要遵照安排，而是因为他们常常忘记、吃药物的安排没被描述清楚、效率低、副作用太严重了等等）。所以，我们有意解释这些问题：

* 帮病人想起来什么时候要吃药物 （利用普通药盒子，最重要的是不管硬件出任何问题，病人还必须吃上他们的药物），
* 为了让病人汇报药物的效率、副作用提供一个最简单方法（这个系统需要减少医生的压力），
* 给医生提供一个简单网上系统为了关照他们的病人的情况（与其靠给病人打电话、发一封信不如用更简单、更效率的方法），
* 最后，假如出任何问题（比如说病人意外地吃下错的药物、忘记吃或者遇到一些副作用），西庸应该提到很方便方法为了给医生发一个通知。

自然，我们设计的系统并不是完全可靠的，我们只想证明这种系统是能建构的，而我们想探索些新部件、新编程语言和平台。

## 电路设计
![基础电路构建](/assets/2019-10-28-hackmanchester/b0.png){:class="img-responsive"}

这个电路包括五个独立配件：
* (1) 项目核心是8266MCU部件—虽然它没有足够输出，但是还合适这个比较简单的hack，
* (2) 为了把项目连接上GSM网络，我们使用SIM800L GSM部件，与大家都习惯的相反，它使用“古代”串行接口，
* (3), (3ʹ) 每一个药盒子都包括三个LED，两个发看得见光的，一个发红外光的；另外，每一个药盒子有感红外的传感器，
* (4) 为了表示通知，我们用最简单方法：LED，
* (5) 最后，我们需要加上三个按钮，可是8266没有足够输入脚，只剩下一个模拟输入；因此，我们不得不需要加上电压分配，然后利用MCU中的A0脚（我们承认是个该死的hack）。

再说，另一个重点是SIM800L部件需要3.7-4.1V电源，从8266可以获得的3.3V电源是完全无法利用的。一开始我们决定了利用这个3.3V电源，SIM800L好像没有问题，可是接不上GSM服务。我们在把它调试的时候，我们注意到SIM800L纷纷发出`SMS Ready`，接下来`Call Ready`，然后（我们估计为了电源电压太低）被重启了。注意，SIM800L最大电流需求上2A，从而，我们不得不加上了一个高电流的电压稳压器[MIC29150](http://www.farnell.com/datasheets/94451.pdf)。

![第一版](/assets/2019-10-28-hackmanchester/step0.jpeg){:class="img-responsive"}

![第二版](/assets/2019-10-28-hackmanchester/step1.jpeg){:class="img-responsive"}

## 软件设计
当然，还需要给MCU跑固件：多亏ESP8266和ESP32的编译器是比较现代的C++标准，除了一些最新对象、算法
（比如`std::optional`、`std::tie`等等）以外，我们还可以靠许多C++ `std` namespace里的东西。TODO：干嘛？

所以，写固件的时候应该按照一般“clean code”的规则，尤其加上仔细的测试。不过，拿利用些MCU里的硬件的功能的代码来说，
在x86处理器进行下到底怎么来写细节的测试代码？比如，我们靠`SoftwareSerial`来连接GSM配件，就算你的用x86处理器的
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