#pragma once

struct HtmlContent {
private:
  const String htmlHeader = R"(
<!DOCTYPE HTML><html><head>
  <meta charset='utf-8'/><meta language='zh_CN'/>
</head><body>
<h1>NFC</h1>
)";

  const String htmlFooter = R"(
</body></html>
)";

  const String programForm = R"(
<form action='/program' accept-charset='utf-8' method='post'>
  <input type='text' name='text'/>
  <input type='submit'/>
</form>
)";
public:
  String home(const String& currentText) {
    return htmlHeader + "<p>" + currentText + "</p>" + programForm + htmlFooter;
  }

} HtmlContent;

