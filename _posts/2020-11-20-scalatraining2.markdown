---
layout: post
title:  "Scala入门培训：for循环"
date:   2020-11-05 11:00:00 +0000
categories: [Scala]
excerpt: "。。。"
hidden: true
---

第一个Scala教程结束之后，我觉得大家对`for`循环和`map`、`flatMap`、`filter`的关系还有点模糊。其实，我也觉得大家不十分清楚`map`、`flatMap`、`filter`到底是什么样的函数，不太了解参数和返回值的类型。然而，大家熟悉命令式编程语言，因此函数式的变换/映射、滤除比较难学的。我希望了看下面的图会解释一切。

![](/assets/2020-11-20-scalatraining2/fmfm.png)

很明显的，第一个操作是滤除——`filter`、第二个是映射——`map`、第三个是`flatMap`。即使我没把变量的类型定义好，但是可以看出`a`和`A`都是一种数组，更确切地说可以从头到尾把每一个元素都读取，即Scala所定义`Iterable[+A]`的子类。那么，从来没遇到函数式的编程语言的人会怎么理解这三个函数呢？我以为上面的图标太明显的，并不需要补充解释；大家竟然不了解`map`、`flatMap`、`filter`参数的类型要求，也不太清楚返回值是怎么算出的。

所以，我们是从普遍Java代码开始的，而不是直接跑到简洁`List(1, 2, 3, ...).map(...)`的。

{% highlight Java linenos %}
private boolean f3(int input) {
    return i > 3;
}

private int f2(int input) {
    return input * 10;
}

private List<Integer> f3(int input) {
    return Arrays.asList(input, input * 10);
}

...

List<Integer> numbers    = ... // 比如 { 1, 2, 3, 4, 5 };
List<Integer> filtered   = new ArrayList<>();
List<Integer> mapped     = new ArrayList<>();
List<Integer> flatMapped = new ArrayList<>();
for (Integer n : numbers) {
    final boolean accept             = f1(n);
    final int mappedValue            = f2(n);
    final List<Integer> mappedValues = f3(n);

    if (accept) filtered.add(n);
    mapped.add(mappedValue);
    for (Integer fm : mappedValues) flatMapped.add(fm);
}
// filtered等于"ArrayList { 4, 5 }"
// mapped等于"ArrayList { 10, 20, 30, 40, 50 }"
// flatMapped等于"ArrayList { 1， 10, 2， 20, 3， 30, 4， 40, 5， 50 }"
{% endhighlight %}

上面的代码是很容易了解的，而且是大学CS教程中很常见的代码。每一个命令都很明显地表示它的目的，很明显地表示它的输入、输出；虽然有点啰嗦，但是容易了解。只不过大部分的Scala源码所使用不是上述的命令式的方法，而使用函数式的方法。`filter`、`map`、`flatMap`的概念虽然很容易了解，但是实现时遇到些问题。学会了基本的概念、算法过后，下一步要学习哪些类型包括上述的特殊函数，而且这三个函数的具体用途是什么。最常见类型是`Option`、`List`、`Either`，因此我们是从这三个具体类型开始学习的。下面的图所解释的是*具体*`map`和`flatMap`的用法。

![](/assets/2020-11-20-scalatraining2/fmfmt.png)

要注意的是，即使图上是把完全不同的类型树（`IterableOps`的子类`Option`、`List`跟`Either`）混合起来；虽然不准确，但理由是仔细地说明这三个又基础又常见的函数的目的和用法。即使继承的结构比较复杂，`map`和`flatMap`的用法也完全一致：一旦熟悉，大家就凭直觉知道怎么用。因为`List`、`Option`、`Either`都是一种容器、都好像是由两个案例类[^1]而定义的，上面的图还所示的是`fold`；我要强调的“容器性“——`Option`、`List`、`Either`都并不是单值，要把它们变换成单值时必须管理所有可能的容器值。换句话说，我们必须把容器的两个案例值`fold`成一个值。

## `for`循环
{% highlight Scala linenos %}
val maybeInt: Option[Int] = Some(1)
val listInt: List[Int] = List(1, 2, 3, 4, 5)

val _ for {
  int <- maybeInt
} yield int * 2

val _ = for {
  int <- maybeInt
  if int > 10
} yield int * 2

val _ = for {
  a <- listInt
  b <- listInt
  if a < 5 && b > 1
} yield a * b
{% endhighlight %}

{% highlight Scala linenos %}
maybeInt.withFilter(int => int > 10).map(int => int * 2)

listInt.flatMap(a => listInt.withFilter(b => b > 1 && a < 5).map(b => a * b))
{% endhighlight %}

[^1]: Haskell、ML把该结构叫做代数数据结构，Haskell、ML都用比较简洁句法来定义代数数据结构，对比一下`sealed trait O[+A]; case class S[A](a: A) extends O[A]; case object N extends O[Nothing]`和`data O a = S a | N`。

<!--
且十分正常。对吧？呵呵，得看情况，更具体地说，在多线情况下上面的代码会出race condition。该race condition会被`numbers`数组同时变、读取引起的。那么，Java包括一个比较有意思的关键词，即`final`。大家都知道定义为`final`代表着变量的值是不可变的，那么不可变的变量怎么能调用类似于`add`的函数呢？其实，在Java，更正确地说JVM，把`final`定义为“指针”是不可变的，而指针所指的实例不是因为`final`而受到任何限制的。

{% highlight Java linenos %}
final List<Integer> numbers    = Arrays.asList(1, 2, 3, 4, 5);
final List<Integer> filtered   = new ArrayList<>();
final List<Integer> mapped     = new ArrayList<>();
final List<Integer> flatMapped = new ArrayList<>();

// 虽然定义为final，还能调用`add`方法
{% endhighlight %}

这里要对比一下C++所带来的`const`。C++的`const`确实是代表着常数、不可变的值。

{% highlight Java linenos %}
const std::vector<int> numbers = { 1, 2, 3, 4, 5 };
numbers.push_back(6);   // 编译时报错：*const* std::vector<int> 不包含着push_back函数
{% endhighlight %}

为什么要谈谈`final`、不可变的变量呢？因为可以把`final`直接翻译到Scala的`val`。我强烈推荐大家都尽量使用`val`；通过`val`可以写更容易了解的代码，一旦我们为某一个定义为`val`的变量赋值好，变量的值不会被改变的。把所有的变量都定义为`val`，一旦有一定的值，我们确定它的值不会再变。可惜，在JVM语言中，`final`本身不能保证不可变性。因为上面描述的`final`规则，我们还必须依赖不可变的数据结构。

通过某一个算法 𝑓 把每一个元素映射另外一个元素

通过某一个算法 𝑓 把每一个元素映射另外一个数组，然后把返回的数组里的每一个元素添加于结果
-->

# 三、高阶Scala

# 四、极高阶Scala

# 五、总结
- 😐 模式匹配
- 🙁 泛型类、包括标准库中`List`、`Option`；标准库的常用方法`map`、`flatMap`、`filter`
- ☹️ 有着两个（以上）类型参数的泛型类、比如标准库中`Either`
- 😬 `for`，包括`for`跟`map`、`flatMap`、`filter`的关系
- 🤬 Pimp my library、implicits

![](/assets/2020-11-03-intro/http4s-intro.png)

<!---
有时候有人问我“我很想学习一门函数式编程语言，比如Scala；我应该怎么样最好开始这个过程？”。不久以前我只有一个推荐：一边读一本书（比如Scala for the impatient或者XXX），一边在一个小项目实现刚刚学的内容。“太谢谢你了”，一般是最后一条消息，我以为他们开始自己的历程，没有任何再要问的问题。我一直以为Scala是好好学的语言，GitHub上面充满着处理各种各样的开源的项目，我以为文档太繁荣了，只要专心于学习，

为了好好学习Scala、Haskell等等函数式语言我们先要好好学习什么是函数，什么把它好好利用；一旦完全了解函数式编程语言的关键概念是什么，立刻跑到仔细地学习标准库的机构、数据类型、标准函数等等。


# 二、“Hello， world”、标准库…… 然后呢？


大家都知道最基本的程序和大型的程序的差别很大很大


我认为Scala，Haskell世界中的最大的问题就是怎么把上述的过程应用于建立有用的程序。
--->