package tech.threebodies

import java.util.concurrent.TimeUnit

import cats.effect.ConcurrentEffect
import org.http4s.client.blaze.BlazeClientBuilder
import org.http4s.client.Client
import sttp.client3._
import sttp.client3.asynchttpclient.zio._
import sttp.client3.httpclient.zio.HttpClientZioBackend
import zio.{clock, Has, Task, ZIO, ZLayer}
import zio.clock.Clock

import scala.concurrent.ExecutionContext

object HttpClient {
  type Uri = String

  trait Service {
    def get(uri: Uri): Task[String]
  }

  private class SttpService(client: SttpClient.Service) extends Service {
    override def get(uri: Uri): Task[String] = {
      val sttpUri = sttp.model.Uri.unsafeParse(uri)
      val request = basicRequest.get(sttpUri).response(ResponseAsByteArray)
      client.send(request).map(response => new String(response.body))
    }
  }

  private class BlazeService(client: Client[Task]) extends Service {
    import zio.interop.catz._
    override def get(uri: Uri): Task[String] =
      client.expect[String](uri).fold(x => s"- $uri $x", x => s"+ $uri: $x")
  }

  val asyncHttpClient: ZLayer[Any, Throwable, Has[Service]] =
    AsyncHttpClientZioBackend.layer().map(client => Has(new SttpService(client.get)))

  val httpClient: ZLayer[Any, Throwable, Has[Service]] =
    HttpClientZioBackend.layer().map(client => Has(new SttpService(client.get)))

  def blazeClient(implicit E: ConcurrentEffect[Task]): ZLayer[Any, Throwable, Has[Service]] = {
    val ec = ExecutionContext.Implicits.global
    val cb = BlazeClientBuilder[Task](ec).allocated.map(x => new BlazeService(x._1))
    ZLayer.fromEffect(cb)
  }

  def get(uri: Uri): ZIO[Has[Service] with Clock, Throwable, RequestBenchmark] = for {
    start    <- clock.currentTime(TimeUnit.MILLISECONDS)
    response <- ZIO.accessM[Has[Service]](_.get.get(uri))
    end      <- clock.currentTime(TimeUnit.MILLISECONDS)
  } yield RequestBenchmark(uri, end - start, response.length.toLong)

}
