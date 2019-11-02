---
layout: post
title:  "Hack Manchester挑战"
date:   2019-10-28 15:23:44 +0000
categories: [HW, SW]
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
*电路设计*

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
当然，还需要给MCU跑固件：多亏[PlatformIO]()中的ESP8266和ESP32包括现代C++编译器（ESP32是编译器是支持C++17的，ESP8266只是实现C++11的），我们很容易就可以利用现代C++标准的方法。

所以，写固件的时候应该按照所有“clean code”的规则，尤其加上仔细的测试。不过，拿利用些硬件的功能来说，在x86处理器进行下到底怎么来写细节的测试代码？比如，我们靠`SoftwareSerial`来连接GSM配件，就算你的用x86_64处理器的电脑有这么古老串行端口。你写好x86_64测试代码之后把它进行在这么快处理器跑肯定会出”works on my machine“，可是并不在MCU这样的问题。我们可以下载一些虚拟硬件的库，不幸的是这些库一般指支持根本Arduino固件，为了来测试更复杂接口、更复杂MCU能力，我们不得不需要自己写这种虚拟硬件库。

从我们的[GitHub版本库](https://github.com/teroxik/hackmanchester2019-hw)举一个例子：怎么虚拟`SoftwareSerial`，然后怎么来使用这个虚拟`SoftwareSerial`？

{% highlight C++ %}
class SoftwareSerial {
 public:
  void println(const char *);
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
  void println(const char *line) { writes.push_back(string(line)); }
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

这里，`software_serial_mock`是控制虚拟`SoftwareSerial`的接口，每次我们的固件调用`SoftwareSerial.begin(...)`，我们把刚刚调用`SoftwareSerial`对象放在`software_serial_mock`里。

{% highlight C++ %}
using namespace std;
class software_serial_mock {
 private:
   vector<tuple<int, int, int, SoftwareSerial *>> mock_instances;

 public:
  void add(SoftwareSerial *instance, const int baud, const int rx_pin, const int tx_pin) {
    mock_instances.push_back(make_tuple(baud, rx_pin, tx_pin, instance));
  }
  SoftwareSerial *get(const int rx_pin) const;
};
{% endhighlight %}

最简单来说，`software_serial_mock`是一种版本库，我们写测试代码一般会调用这个版本库中`get`方法为了搜索`SoftwareSerial`对象。

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

显然，调查`get_writes()`，返回`std::vector<std::string>`的方法很麻烦；更方便的是加上一种控制接口；把它实现在`SoftwareSerial`与`software_serial_mock`，结果：

{% highlight C++ %}
using namespace std;
class software_serial_control {
 public:
  virtual void rx(const string rx, 
                  const chrono::duration<uint, milli> timeout) = 0;
  virtual void tx(const string tx) = 0;
  virtual void add_auto_rxtx(const string request, 
                             const string response,
                             const chrono::duration<uint, milli> timeout) = 0;
  virtual void set_blocking(const bool blocking) = 0;
};
{% endhighlight %}