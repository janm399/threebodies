---
layout: post
title:  "Scala入门培训：第一文章"
date:   2020-11-03 11:00:00 +0000
categories: [Scala]
excerpt: "。。。"
hidden: false
---

一开始教Scala课程，我问了问实习者是否他们觉得要从最基本概念开始学习。我把最基础概念定义为

- 可变的变量和不可变的变量
- 具体数据类型（`Int`、`String`等等）
- `if-then-else`条件
- `while`循环，`for`作为循环
- 基础数据结构（一般类`class`、案例类`case class`、元组`(...) tuple`)

我确信已熟悉Scala的同学也觉得这些是Scala的基础，而且跟其他编程语言很像的概念。掌握它们，实习者就可以专心于更高级的Scala句法和用法，比如模型匹配、高阶函数（特别是标准库中的常用高阶函数`map`、`filter`、`flatMap`)、介绍高阶类型（`Option`、`List`、`Either`)、（尾）递归函数等等。所以第一节课问的问题“你们要从零开始学习？”太尴尬了。按照考试、面试实习者很明显地已经掌握了这些大部分的编程语言拥有的基础。不管如此，第一个星期我们从头到尾探讨了这些。第一节课我给他们准备一个模版、只有`build.sbt`的Scala项目，然后我们使用IntelliJ IDEA中的Scala REPL来讨论如何用Scala。

![](/assets/2020-11-03-intro/repl.png)

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

好在应该下课了，同学都逃跑了。我在想一切都是表达式可能需要一段时间、亲手写几个例子才能熟悉。我决定了下一节课继续往下走。这里再要提醒：同学都已经熟悉其他编程语言，即Java、Python、JavaScript和TypeScript。

### 循环：`while`、作为循环的`for`

### 类、案例类、元组

# 三、高阶Scala

# 四、极高阶Scala

# 五、总结

![](/assets/2020-11-03-intro/http4s-intro.png)

<!---
有时候有人问我“我很想学习一门函数式编程语言，比如Scala；我应该怎么样最好开始这个过程？”。不久以前我只有一个推荐：一边读一本书（比如Scala for the impatient或者XXX），一边在一个小项目实现刚刚学的内容。“太谢谢你了”，一般是最后一条消息，我以为他们开始自己的历程，没有任何再要问的问题。我一直以为Scala是好好学的语言，GitHub上面充满着处理各种各样的开源的项目，我以为文档太繁荣了，只要专心于学习，

为了好好学习Scala、Haskell等等函数式语言我们先要好好学习什么是函数，什么把它好好利用；一旦完全了解函数式编程语言的关键概念是什么，立刻跑到仔细地学习标准库的机构、数据类型、标准函数等等。


# 二、“Hello， world”、标准库…… 然后呢？


大家都知道最基本的程序和大型的程序的差别很大很大


我认为Scala，Haskell世界中的最大的问题就是怎么把上述的过程应用于建立有用的程序。
--->