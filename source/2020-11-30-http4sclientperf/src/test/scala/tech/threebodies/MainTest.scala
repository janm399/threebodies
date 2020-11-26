package tech.threebodies

import zio.interop.catz._
import zio.test.{DefaultRunnableSpec, _}
import zio.test.Assertion._

object MainTest extends DefaultRunnableSpec {

  // BlazeClient: 2693 - 830 - 894
  // Sttp:        1276 - 900 - 546

  def spec = suite("Http4sClient performance suite")(
    testM("Program makes requests") {
      val p = Main.program.provideLayer(zio.clock.Clock.live ++ HttpClient.sttpBacked)
      for {
        (a, time1) <- p
        (b, time2) <- p
        (c, time3) <- p
        _          <- console.putStrLn(s"$a\n$b\n$c\n\n\n$time1 - $time2 - $time3")
      } yield assert(true)(equalTo(true))
    }
  )
}
