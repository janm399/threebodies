编程涉及

探索  探知

# 避免不确定性
我们写的代码最好避免不确定性：不确定性在用户看来。。。函数式语言雨来应用透明来避免不确定，命令式语言依赖严格顺利来避免不确定性。与命令式语言不同的是，引用透明式、函数式的语言避免另外一个不确定性的根源：（可变的）状态。

## 引用透明
引用透明意味着把函数的调用代替该函数的内部代码后，程序的语义仍然一样。引用透明的最重要的结果是函数千万不能包括直接执行的副作用；换句话说，引用透明的程序只能使用纯函数；数学的意思的“纯函数”：函数的返回值只能够依赖该函数的参数。举例子，如果`def foo(): Int`函数是纯函数，他的返回值必定是一个常数。再考虑吧：`def foo(): Int`没有参数，随着纯函数的定义——纯函数的返回值只能够依赖参数——可见没有参数的函数无法返回非常数。下一个内涵，下一个特性就是能够把纯函数的返回值存贮在缓存里；更抽象地说，每一个纯函数能够被代替与一般`map`，与此同时，每一个`map`能够被代替与一个纯函数。纯函数只是把输入映射输出，千万不包括其他副作用。

{% highlight scala %}
enum Domain {
  case One
  case Two
  case Three
}

enum Codomain {
  case Red
  case Blue
}

object RT {

  import Domain._
  import Codomain._
  def m1(d: Domain): Codomain = d match {
    case One | Two => Red
    case Three => Blue
  }

  val m2: Map[Domain, Codomain] = Map(
    One -> Red, 
    Two -> Red, 
    Three -> Blue
  )

  def test(f: Domain => Codomain): Unit = {
    assert(f(One) == Red)
    assert(f(Two) == Red)
    assert(f(Three) == Blue)
  }

  def main(args: Array[String]): Unit = {
    test(m1)
    test(m2)
  }

}
{% endhighlight %}

引用透明主要结果让人清楚地了解程序的行为，因为引用透明的程序不仅仅没有内部、可变的状态，而且程序中的函数没有任何副作用。

# 什么是副作用？
大可惜的是唯一个原因来编写一个应用就是为了它的副作用。甚至连最聪明、最复杂的纯函数无法影响外面的世界，因为为了影响外面的世界，该函数需要执行一种副作用。经典的例子，经典的玩笑就是`def foo(): Int = { launchNuclearMissiles(); 42 }`，虽然从外面看`def foo(): Int`好像是纯的函数，实现却包括一个“副作用”，即`launchNuclearMissiles()`。

那么，副作用主要点是什么？回想第一句话：我们的最终目的是避免不确定性，我们知道命令式代码依赖”一行一行”地进行的方法来表达该顺序。一步一步地进行的方法难以把计算分布，parallelise。


对比命令式和函数式
