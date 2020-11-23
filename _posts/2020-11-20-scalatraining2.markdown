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
首先，我们探讨“传统”的编程语言中的`for`循环。一如所料，编译器将`for`循环编译成（用着Intel x86_64汇编语言的句法）`mov, add, cmp, jne`——初始化、增量、对比、跳转——的命令顺序。这里我们不考虑一定编译时优化，例如循环展开、SIMD命令等等，主要的是探讨循环的最基本的原则。

{% highlight C++ linenos %}
for (int i = 0; i < 100; i++) {
    // A
    ...
} 
// B
{% endhighlight %}

{% highlight asm linenos %}
    xor     ebx, ebx    // int i = 0
.L1:                        // A
    ...
    add     ebx, 1      // i++
    cmp     ebx, 100    // temp = i == 100
    jne     .L1         // if (!temp) goto .L2
// B
{% endhighlight %}

那么，从原则看来，Java编译器跟C++编译器所生产的机器命令一致。不过除别在于Java虚拟机是所谓栈式器，而其余的处理器包括x86_64、ARM、AVR、PIC等等都是寄存器式。你可以将栈理解为“无限的”、有序的寄存器集合。

{% highlight Java linenos %}
for (int i = 0; i < 100; i++) {
    // A
    ...
} 
// B
{% endhighlight %}

{% highlight asm linenos %}
    iconst_0            // 0                        | 
    istore_1            //                          | (local) i = 0
    iload_1             // 0                        | 
1:
    bipush 100          // 0, 100                   |
    if_icmpge 2 (+16)   //                          | if (100 >= 100) goto 2
    // A
    iinc 1 by 1         //                          | i++
    goto 1 (-16)        //                          | goto 1
2:  // B
{% endhighlight %}


<!--
{% highlight Rust linenos %}
for i in 0..100 {
    // A
    ...
}
{% endhighlight %}

{% highlight asm linenos %}
        xor     ebp, ebp            // i = 0
.L2:
        // A
        ...
        add     ebp, 1              // i++
        cmp     ebp, 100            // temp = i == 100
        jne     .L2                 // if (!temp) goto .L2
// B
{% endhighlight %}

{% highlight Rust linenos %}
for i in 0..<100 {
    // A
    ...
}
// B
{% endhighlight %}

{% highlight asm linenos %}
        xor     r12d, r12d          // i = 0
.L2:
        // A
        lea     r12, [r12 + 1]      // i++
        cmp     r12, 100            // temp = i == 100
        jne     .L2                 // if (!temp) goto .L2
// B
{% endhighlight %}
-->

无论是栈式处理器还是寄存器式处理器，原则上`for`循环都一致。那么，Scala的`for`循环完全不同。它确实并不是一种循环，Scala编译器以并不会把它编译成“初始化、增量、对比、跳转”这条命令顺序。Scala将`for`理解为句法糖，类似于Haskell的`do`句法糖。因此，Scala编译器会将`for`编译成调用`map`或者`foreach`、`flatMap`、`filter`方法。

{% highlight Scala linenos %}
for (i <- 0 until 100) {
  // A
}
{% endhighlight %}

{% highlight asm linenos %}
    iconst_0            // 0                        |
    invokevirtual #67   //                          | intWrapper(0)
    bipush 100          // RichInt(0), 100          |
    invokevirtual #71   //                          | RichInt(0).until(100)
    invokedynamic #89   // Range(0, 100), #89-ret   |
    invokevirtual #95   //                          | Range(0, 100).foreach(I => Unit)
    ...                 // Unit

#67: scala.Predef.intWrapper (I)RichInt
#71: scala.runtime.RichInt.until (RichInt, Int)Range
#89: <anonymous> (I)V { // A }
#95: Range.foreach ((I)V)V
{% endhighlight %}

也就是说`for (i <- 0 until 100) println(i)`确实被编译成`intWrapper(0).until(100).foreach(x => println(x))`。`intWrapper(0).until(100)`是`0 until 100`通过Implicit Resolution所生产的，接下来的`foreach`是不包括`yield`的`for`句法糖的最终结果。关键词`yield`控制句法糖所生产的最终调用的方法：没有`yield`的话，最终调用的方法是`foreach`（因此，`for`表达式的返回类型是`Unit`，返回值是`()`）；有`yield`的话，最终调用的方法是`map`（因此，`for`表达式的返回值是`map`所返回的值）。

<table class="rouge-table">
<tr>
    <th>循环句法糖</th>
    <th>编译的结果</th>
</tr>
<tr>
    <td>
{% highlight Scala %}
for (i <- 0 until 100) yield i * 2
{% endhighlight %}
    </td>
    <td>
{% highlight Scala %}
(0 until 100).map(i => i * 2)
{% endhighlight %}
    </td>
</tr>
<tr>
    <td>
{% highlight Scala %}
for {
  i <- 0 until 100
  if i > 100
} yield i * 2
{% endhighlight %}
    </td>
    <td>
{% highlight Scala %}
(0 until 100)
  .withFilter(i => i > 100)
  .map(i => i * 2)
 
{% endhighlight %}
    </td>
</tr>
<tr>
    <td>
{% highlight Scala %}
for {
  a <- 0 until 10
  b <- 1 until 20
  if a < 5 && b > 1
} yield a * b
 
{% endhighlight %}
    </td>
    <td>
{% highlight Scala %}
(0 until 10)
  .flatMap { a => 
    (1 until 20)
      .withFilter(b => b > 1 && a < 5)
      .map(b => a * b)
  } 
{% endhighlight %}
    </td>
</tr>
</table>

上面的表所示的编译器如何将句法糖编译非句法糖的代码；因为最终生产的字节码只能依靠Java虚拟机而执行，Scala编译器所输出的字节码只能依靠普遍方法调用。*循环本身的算法必须实现在`map`、`flatMap`、`foreach`等的方法*。作为一个例子，下面`List.foreach`的远吗很明显地表示循环：

{% highlight Scala %}
@inline final override def foreach[U](f: A => U): Unit = {
  var these = this
  while (!these.isEmpty) {
    f(these.head)
    these = these.tail
  }
}
{% endhighlight %}

## `for`循环句法糖的好处
好像传统的`for`循环所生产的机器命令很简洁；这简洁也包括Java虚拟机的命令。反而Scala好像不支持`for`循环，而是通过`while`循环·来模拟`for`循环。不过，从底层命令来看，`for`和`while`没有任何区别，二者都是通过判断一个条件来算出专挑的目的。那么，虽然理论上`for`和`while`没有任何区别，但是Scala的`for`式方法一定比传统`for`慢得多；比传统`for`循环要通过更多`INVOKEVIRTUAL`命令而执行。简单来说`INVOKEVIRTUAL`命令会通过虚拟方法表找到具体方法来调用。


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
