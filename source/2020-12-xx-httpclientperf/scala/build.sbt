val Http4sVersion     = "0.21.11"
val CirceVersion      = "0.13.0"
val Specs2Version     = "4.10.5"
val LogbackVersion    = "1.2.3"
val ZIOVersion        = "1.0.3"
val ZIOInteropVersion = "2.2.0.1"
val SttpVersion       = "3.0.0-RC10"

lazy val root = (project in file("."))
  .enablePlugins(NativeImagePlugin)
  .settings(
    organization := "tech.threebodies",
    name := "httpclientperf",
    version := "0.1.1-SNAPSHOT",
    scalaVersion := "2.13.3",
    testFrameworks := Seq(TestFrameworks.ScalaTest),
    Compile / mainClass := Some("tech.threebodies.Main"),
    scalacOptions := Seq(
      "-feature",
      "-deprecation",
      "-explaintypes",
      "-unchecked",
      "-encoding",
      "UTF-8",
      "-language:higherKinds",
      "-language:existentials",
      "-Xfatal-warnings",
      "-Xlint:-infer-any,_",
      "-Ymacro-annotations",
      "-Ywarn-value-discard",
      "-Ywarn-numeric-widen",
      "-Ywarn-extra-implicit",
      "-Ywarn-unused:_",
      "-opt:l:inline",
      //"-opt-inline-from:**",
      "-Xprint:typer",
      "-P:bm4:no-filtering:y",
      "-P:bm4:no-map-id:n"
    ),
    libraryDependencies ++= Seq(
      "org.http4s"                    %% "http4s-blaze-server"           % Http4sVersion,
      "org.http4s"                    %% "http4s-blaze-client"           % Http4sVersion,
      "org.http4s"                    %% "http4s-circe"                  % Http4sVersion,
      "org.http4s"                    %% "http4s-dsl"                    % Http4sVersion,
      "io.circe"                      %% "circe-generic"                 % CirceVersion,
      "ch.qos.logback"                 % "logback-classic"               % LogbackVersion,
      "dev.zio"                       %% "zio"                           % ZIOVersion,
      "dev.zio"                       %% "zio-interop-cats"              % ZIOInteropVersion,
      "com.softwaremill.sttp.client3" %% "core"                          % SttpVersion,
      "com.softwaremill.sttp.client3" %% "async-http-client-backend-zio" % SttpVersion,
      "com.softwaremill.sttp.client3" %% "httpclient-backend-zio"        % SttpVersion,
      // "org.scalameta"                 %% "svm-subs"                      % "20.2.0",
      "org.specs2" %% "specs2-core"       % Specs2Version % "test",
      "dev.zio"    %% "zio-test"          % ZIOVersion    % "test",
      "dev.zio"    %% "zio-test-sbt"      % ZIOVersion    % "test",
      "dev.zio"    %% "zio-test-magnolia" % ZIOVersion    % "test" // optional
    ),
    addCompilerPlugin("com.olegpy" %% "better-monadic-for" % "0.3.1")
  )
