package tech.threebodies

import java.io.BufferedWriter

import zio.{UIO, ZIO}

final case class Benchmark(steps: List[BenchmarkStep]) {
  def writeTo(out: BufferedWriter): UIO[Unit] =
    ZIO.foreach_(steps)(_.writeTo(out))
}

sealed trait BenchmarkStep {
  def writeTo(out: BufferedWriter): UIO[Unit]
}
object BenchmarkStep {

  final case class Sleep(time: Long) extends BenchmarkStep {
    override def writeTo(out: BufferedWriter): UIO[Unit] = UIO {
      out.write(s"sleep,$time\n")
    }
  }
  final case class RunRequests(responses: List[RequestBenchmark], elapsedTime: Long) extends BenchmarkStep {
    import RequestBenchmark._
    def writeTo(out: BufferedWriter): UIO[Unit] = for {
      _ <- ZIO.foreach_(responses.sorted(Ord))(_.writeTo(out))
      _ <- UIO(out.write(s"requests,$elapsedTime\n"))
    } yield ()

  }

}

final case class RequestBenchmark(uri: String, elapsedTime: Long, responseSize: Long) {
  def writeTo(out: BufferedWriter): UIO[Unit] = UIO {
    out.write(s"request,$uri,$elapsedTime,$responseSize\n")
  }
}
object RequestBenchmark {
  implicit object Ord extends Ordering[RequestBenchmark] {
    override def compare(x: RequestBenchmark, y: RequestBenchmark): Int = x.uri.compareTo(y.uri)
  }
}
