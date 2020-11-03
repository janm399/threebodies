---
layout: post
title:  "Scala入门培训：基础"
date:   2020-11-01 11:00:00 +0000
categories: [Scala]
excerpt: "。。。"
hidden: true
---

一开始教Scala课程，我问了问实习者是否他们觉得要从最基本概念开始学习。我把最基础概念定义为

- 可变的变量和不可变的变量
- 具体数据类型（`Int`、`String`等等）
- `if-then-else`条件
- `while`循环，`for`作为循环
- 基础数据结构（一般类`class`、案例类`case class`、元组`(...) tuple`）

我确信已熟悉Scala的同学也觉得这些是Scala的基础，而且跟其他编程语言很像的概念。掌握它们，实习者就可以专心于更高级的Scala句法和用法，比如模型匹配、高阶函数（特别是标准库中的常用高阶函数（`map`、`filter`、`flatMap`）、介绍高阶类型（`Option`、`List`、`Either`）、（尾）递归函数等等。所以第一节课问的问题“你们要从零开始学习？”太尴尬了。按照考试、面试实习者很明显地已经掌握了这些大部分的编程语言拥有的基础。不管如此，第一个星期我们从头到尾探讨了这些。第一节课我给他们准备一个模版、只有`build.sbt`的Scala项目，然后我们使用IntelliJ IDEA中的Scala REPL来讨论如何用Scala。

![](/assets/2020-11-03-scalatraining1/repl.png)

我们的进步非常快：一个星期内我们把所有上述的基础学会了。第一捷克我觉得我在小看他们的能力，我一直提醒了自己“看他们的考试吧，他们已经知道如何编译，不要浪费时间，不要给他们教变量、`String`、`Int`、条件”。不管如此，教了两个小时的课后，我确信实习者都完全了解变量和常数、具体数据类型、条件。唯一个新学的东西是Scala中一切都是表达式，也就是说甚至连`if`都是计算为一个具体的值的。下面的清单所示是第一节课中讨论的关键例子。(再提醒：所有的例子是通过Scala REPL而执行的。)

### 定义（可变的和不可变的）变量，即基础类型
{% highlight Scala linenos %}
// 手动定义的类型
val number11: Int = 11  // 可以把它想象Java中final int number11 = 11;
val number12      = 11  // 可以把它想象Java中final int number12 = 11;

// 类型推断
var number21: Int = 12  // 可以把它想象Java中      int number21 = 12;
var number22      = 12  // 可以把它想象Java中      int number22 = 12;
{% endhighlight %}

教变量定义没遇到任何问题，同学都很清楚。熟悉Java的同学觉得Scala的句法有点奇怪，不过自信地说到“习惯了后就好了”。这时我提出了其他一样“奇怪”的编程语言，比如Go、TypeScript、即Haskell。反应都是“明白了”，可以往下走。同时我们探讨了`Int`、`String`、`Boolean`等等基础类型。我们实现了几个简简单单的例子：数字的算法，`String`的算法等等。返回又是“明白了”。

### 条件，即一切都是表达式
往下走我们谈了谈如何使用Scala条件的句法和用法。第一个例子跟命令式语言一致；不足为奇，没有任何问题，大家都遇到了其他编程语言中的类似结构。

{% highlight Scala linenos %}
val n = 11
if (n % 15 == 0) {
  println("fizz buzz")
} else if (n % 5 == 0) {
  println("buzz")
} else if (n % 3 == 0) {
  println("fizz")
} else {
  println(n.toString)
}
{% endhighlight %}

第二个例子更加复杂，是即`if-then-else`是个表达式，那么它的类型是什么？具体地讲，下面的清单中的`fb`是什么类型？

{% highlight Scala linenos %}
val n = 11
val fb = if (n % 15 == 0) "fizz" else "buzz"
{% endhighlight %}

这是第一次我们遇到不太了解的问题。我的解释的方法是让他们回想“Scala中一切都是表达式”，因此`if-then-else`必须可以算出它的值，而且把算出的值给某一个变量赋。要把它想为`if (条件) T else E`，返回的值是依赖条件是否等于`true`，返回的类型是`T`和`E`的...这次反应不是“明白了”，而是太不确定的”OK吧“。下课的时候快要到了，我认为通过实现`abs`函数可以更容易地了解`if-then-else`是有值的概念。可惜的是下面的代码我觉得是完全模糊的。

{% highlight Scala linenos %}
def abs(n: Int): Int = if (n < 0) -n else n
{% endhighlight %}

这让我想一想：一切都是表达式可能需要一段时间、亲手写几个例子才能熟悉。我决定了下一节课继续往下走。这里再要提醒：同学都已经熟悉其他编程语言，即Java、Python、JavaScript和TypeScript。

### 循环：`while`、作为循环的`for`
接下来我们讨论了循环，`while`是比较容易理解的，它是跟Java、Python等等中的`while`完全一致。唯一个不同的是Scala并没有`break`关键词。不仅如此，大家都了解`while`是怎么操作的，而`for`循环恐怕不那么简单。大家都熟悉传统、可变式的`for`，而在Scala使用最基本的`for`竟然涉及更复杂的句法。

{% highlight Scala linenos %}
for (i <- 0 until 10) {
  println(i)
}
{% endhighlight %}

这里，熟悉Python的人不觉得太奇怪了，他们回想Python中`for range`的结构。其余直接接受了另外一个Scala特点；将`for (i <- 0 until 10)`直接翻译成`for (i = 0; i < 10; i++)`就好了。我指出了`0 until 10`确实是一个普遍的值、一个普遍的实例，而且它的类型应该是某一个抽象数组。重构的结果如此:

{% highlight Scala linenos %}
val range = 0 until 10
for (i <- range) { ... }
{% endhighlight %}

问到”range是什么类型？”也没有人像回答。我把`0 until 10`代替`List(0, 1, 2, ..., 9)`，这样希望把例子解释得更清楚，提出`range`只应该是一种可以从头到尾的、一步一步读取的结构。因此，`List`、`Iterable`、`Seq`都可以在`for`循环使用。问题来了呢，“这个`until`是一种关键词、一种句法的特性吗？“———— 想一想，你会如何回答？一个可能是“现在只要知道怎么用，就好了；我们后来会仔细地看看”，另一个是承认情况其实有点复杂。前者对同事特别屈尊，而后者会打开Scala句法的大门。

这里我犯了大错误：我以为我应该给同事们解释`0 until 10`到底是什么。首先，我把`0 until 10`改变到`0.until(10)`；我的目的是说明`0 until 10`这表达式并不是任何神秘，而是普遍的方法调用。我应该就说完，但我继续讲。“想象，”我很热情地说道，“我们可以将每一个有着*一个*参数的方法作为一个算子，算子用法意味着不要加`.`，而不要加圆括号。” 我又应该就说完，但我继续讲，要指出`==`、`>`、`!=`都跟`until`很像：它们都虽然名字很奇怪，但还是有着*一个*参数的方法。结果用法跟`until`一样，而且它们的定义便是最普遍的方法。

{% highlight Scala linenos %}
final abstract class Int private extends AnyVal {
  ...
  def ==(x: Int): Boolean
  def >=(x: Int): Boolean
  def <=(x: Int): Boolean
  def !=(x: Int): Boolean
  ...
}
{% endhighlight %}

有着一个定义为`Int`类型的值，判断它等不等于某一个其他`Int`便是直接调用`def ==(x: Int): Boolean`方法。因此可以把`1 == 2`改编成`1.==(2)`，而且返回类型一定是`Boolean`、返回值一定是`false`。好在没有人问我实现的代码在哪里呢。

愚蠢还没结束，其实刚刚开始的。熟悉Scala的同学肯定知道下一个要介绍的句法特性是什么。没错：Pimp my library[^1]！`0 until 1`（也就是`0.until(1)`）意味着`Int`类型应该有一个叫`until`的方法。但是查看`scala.Int`类型我们找不到`until`。那么，`until`到底在哪里，而且编译器是怎么找到它呢？

{% highlight Scala linenos %}
object Predef {
  ...
  @inline implicit def intWrapper(x: Int): runtime.RichInt = new runtime.RichInt(x)
  ...
}
final class RichInt(val self: Int) extends AnyVal ... {
  ...
  def until(end: Int): Range = Range(self, end)
  ...
}

val range = intWrapper(1).until(10)
{% endhighlight %}

骑虎难下，为了更清楚的介绍我点击了IntelliJ IDEA的【试图】 -> 【显示隐式提示】，如下面图所示。

![](/assets/2020-11-03-scalatraining1/implicits.png)

那时，可怜的实习者都失望了；仿佛要大喊”以前我以为`for`、`while`是可以习得的，我心理做好好准备要学习新的句法，我也准备好了忍受一些复杂的、一些奇怪的东西。但现在呢我只要放弃。“

![](http://abstrusegoose.com/strips/computer_science_major.png)

换句话说“WTF man，我只要学习怎么用`for`。“

### 类、案例类、元组
普遍`class`不应该那么复杂，Scala `class`跟Java `class`很像——更简洁Java会有多难呢？*动物*类和它的两个字累*猫*和*狗*有些古典美，也很容易理解。

{% highlight Scala linenos %}
abstract class Animal {
  def live(): Unit = println("我还活着")
}
class Dog extends Animal {
  def bark(): Unit = ...
}
class Cat extends Animal {
  def miaow(): Unit = ...
}

val c: Cat = new Cat
val a: Animal = c
a.live()    // OK
a.miaow()   // 编译时的错误：Animal类不包含miaow()方法
c.miaow()   // OK
{% endhighlight %}

最重要对象式编译的规定（继承、抽象类、抽象方法）大家都很熟悉，但我发现了有些规定还有点模糊，比如`class`跟`trait`的继承，抽象类跟`trait`的差别，更不要说它们的用法。不管如此，这个古典例子与CS的课程常见CS课程很般配。下一个话题是最好数据“容器”是什么？这里我提出了其他编程语言的类似于`struct`的结构，总之我们想表达的结构是普遍数据容器，该容器应该是最方便、最简洁表达办法。考虑这两个要求，Java式`class`并不适合。为了证明这一点，我所写的是Java式`class`，如下面清单所示。

{% highlight Scala linenos %}
class Dog(name: String, age: Int) {
  def getName: String = name
  def getAge: Int     = age
}
{% endhighlight %}

上面的清单也很容易理解的，大家都很喜欢Scala所介绍的简洁句法。方法定义，虽然一开始大家都觉得有点奇怪（好像全部翻过来的：把类型定义要放在标识符的后面，没有参数的方法不需要加圆括号，在方法体只有一个表达式下，不需要加大括号），但除此之外没有任何不理解的代码。我以为大家都已经遇到Java式数据容器所带来的麻烦，即缺乏正常的`equals`和`hashCode`、也缺乏方便`toString`。原来大家都没遇到下面清单所示的问题。

{% highlight Scala linenos %}
val a1 = new Dog("猎犬", 8)
val a2 = new Dog("猎犬", 8)
val b1 = new Dog("乖乖", 1)

b1 == a1    // 正常，false
a1 == a1    // 正常，true
a1 == a2    // 不正常，false

a1.toString() // 不正常，"...Dog@715f45c6"
{% endhighlight %}

好像大家都没意识到普遍Java式的类不仅仅不包括`equals`、`hashCode`、`equals`，而且没意识到这三个方法的重要性。手动的实现也太麻烦了，再加大部分的开发工具提供自动生产的功能，结果“有没有办法让编译器自动地把它们生产？”是很自然的问题。

# 总结
总体来看，从最容易理解的句法、用法到最难理解的是：

- 🙂 变量、函数、没有类型参数的类（`trait`、`class`）、方法的定义
- 🙂 条件、基础数据类（`Int`、`String`等等）
- 😐 `object`和`(case) class`、`trait`的差别
- 😐 基础、非泛类的案例类、不可变的重要性、`copy`方法
- 😐 模式匹配
- 🙁 泛型类、包括标准库中`List`、`Option`；标准库的常用方法`map`、`flatMap`、`filter`
- ☹️ 有着两个（以上）类型参数的泛型类、比如标准库中`Either`
- 😬 `for`，包括`for`跟`map`、`flatMap`、`filter`的关系
- 🤬 Pimp my library、implicits



[^1]: [Pimp my library](https://www.artima.com/weblogs/viewpost.jsp?thread=179766)