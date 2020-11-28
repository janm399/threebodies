package tech.threebodies

import java.io.{BufferedWriter, FileWriter}
import java.util.concurrent.TimeUnit

import zio.{ExitCode, Has, UIO, ZIO}
import zio.clock.Clock
import zio.console.Console
import zio.duration.durationInt
import zio.interop.catz._

import scala.io.Source

object Main extends zio.App with CatsApp {
  type Env = Console with Clock
  val getUris: UIO[List[HttpClient.Uri]] = UIO(Source.fromFile("../uris.txt").getLines().toList)

  val oneRun: ZIO[Clock with Has[HttpClient.Service], Throwable, BenchmarkStep.RunRequests] = for {
    requests  <- getUris
    start     <- zio.clock.currentTime(TimeUnit.MILLISECONDS)
    responses <- ZIO.collectAllPar(requests.map(HttpClient.get))
    end       <- zio.clock.currentTime(TimeUnit.MILLISECONDS)
  } yield BenchmarkStep.RunRequests(responses, end - start)

  val program = for {
    a <- ZIO.loop(0)(_ < 10, _ + 1)(_ => oneRun)
    _ <- zio.clock.sleep(10.seconds)
    b <- ZIO.loop(0)(_ < 10, _ + 1)(_ => oneRun)
    _ <- zio.clock.sleep(80.seconds)
    c <- ZIO.loop(0)(_ < 10, _ + 1)(_ => oneRun)
    _ <- ZIO(new BufferedWriter(new FileWriter("out.txt")))
      .bracket(x => UIO(x.close()))
      .apply(Benchmark(a ++ b ++ c).writeTo)
  } yield ()

  override def run(args: List[String]): ZIO[Env, Nothing, ExitCode] = {
    program
      .provideLayer(Clock.live ++ HttpClient.httpClient)
      .map(_ => ExitCode.success)
      .orDie
  }
}
