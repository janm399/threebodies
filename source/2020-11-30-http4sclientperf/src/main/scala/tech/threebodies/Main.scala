package tech.threebodies

import java.util.concurrent.TimeUnit

import zio.{ExitCode, Has, UIO, ZIO}
import zio.clock.Clock
import zio.console.Console
import zio.interop.catz._

import scala.io.Source

object Main extends zio.App with CatsApp {
  type Env = Console with Clock
  val getRequests: UIO[List[HttpClient.Uri]] = UIO(Source.fromFile("requests.txt").getLines().toList)

  val program: ZIO[Clock with Has[HttpClient.Service], Throwable, (List[String], Long)] = for {
    requests  <- getRequests
    start     <- zio.clock.currentTime(TimeUnit.MILLISECONDS)
    responses <- ZIO.collectAllPar(requests.map(HttpClient.get))
    end       <- zio.clock.currentTime(TimeUnit.MILLISECONDS)
  } yield (responses, end - start)

  override def run(args: List[String]): ZIO[Env, Nothing, ExitCode] = {
    program
      .provideLayer(Clock.live ++ HttpClient.blazeBacked)
      .map(_ => ExitCode.success)
      .orDie
  }
}
