lazy val root = (project in file("."))
  .settings(
    organization := "tech.threebodies",
    name := "scalatraining2",
    version := "0.1.1-SNAPSHOT",
    scalaVersion := "2.13.3",
    testFrameworks := Seq(TestFrameworks.ScalaTest),
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
      "-opt-inline-from:**",
      "-Xprint:typer",
      "-P:bm4:no-filtering:y",
      "-P:bm4:no-map-id:n"
    ),
    addCompilerPlugin("com.olegpy" %% "better-monadic-for" % "0.3.1")
  )
