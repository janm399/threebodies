package tech.threebodies

sealed trait Maybe[+A] {
  def flatMap[B](f: A => Maybe[B]): Maybe[B]
  def map[B](f: A => B): Maybe[B]
  // def withFilter(f: A => Boolean): Maybe[A]
}
object Maybe {
  case class Some[A](value: A) extends Maybe[A] {
    override def flatMap[B](f: A => Maybe[B]): Maybe[B] = this match {
      case Some(a) => f(a)
      case _       => None
    }
    override def map[B](f: A => B): Maybe[B] = this match {
      case Some(a) => Some(f(a))
      case _       => None
    }

//    override def withFilter(f: A => Boolean): Maybe[A] = this match {
//      case Some(a) if f(a) => this
//      case _               => None
//    }
  }
  case object None extends Maybe[Nothing] {
    override def flatMap[B](f: Nothing => Maybe[B]): Maybe[B] = None
    override def map[B](f: Nothing => B): Maybe[B] = None
//    override def withFilter(f: Nothing => Boolean): Maybe[Nothing] = None
  }
}

object Main {

  def main(args: Array[String]): Unit = {
    val maybeInt: Maybe[Int] = Maybe.Some(1)
    val listInt: List[Int] = List(1, 2, 3, 4, 5)

    val _ = for {
      a <- listInt
      b <- listInt
      if a < 5 && b > 1
    } yield a * b

    val _ = for {
      a <- maybeInt
      b <- maybeInt
      //if int > 10
    } yield a * b

  }

}
