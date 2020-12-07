#include <boost/asio/strand.hpp>
#include <ctime>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>
using mc = boost::posix_time::microsec_clock;

using namespace std;  // so shoot me

struct request_benchmark {
  string uri;
  long elapsed_time;
  long response_size;

  friend ostream &operator<<(ostream &os, const request_benchmark &o) {
    os << o.uri << "," << o.elapsed_time << "," << o.response_size << endl;
    return os;
  }
};

struct run_requests {
  vector<request_benchmark> responses;
  long elapsed_time;

  friend ostream &operator<<(ostream &os, const run_requests &o) {
    auto responses = o.responses;
    sort(begin(responses), end(responses),
          [](auto const& a, auto const& b) { return a.uri < b.uri; });
    for (const auto &response : responses) os << response << endl;
    os << o.elapsed_time << endl;
    return os;
  }
};

class benchmark {
private:
  net::io_context ioc;
  ssl::context ctx;
  tcp::resolver resolver;
  vector<string> uris;

public:
  benchmark();

  request_benchmark one_run(const string &host, const string &port,
                            const string &target);
};

benchmark::benchmark(): ctx{ssl::context::tlsv12_client}, resolver(ioc) {
  ctx.load_verify_file("/etc/ssl/cert.pem");
  ctx.set_verify_mode(ssl::verify_peer);
  ifstream uri_file("../uris.txt");
  copy(istream_iterator<string>(uri_file), istream_iterator<string>(),
       back_inserter(uris));
}

request_benchmark benchmark::one_run(const string &host, const string &port,
                                     const string &target) {
  constexpr int version = 11;

  const auto start = mc::local_time();

  beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
  if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
    beast::error_code ec{static_cast<int>(::ERR_get_error()),
                         net::error::get_ssl_category()};
    throw beast::system_error{ec};
  }

  auto const results = resolver.resolve(host, port);
  beast::get_lowest_layer(stream).connect(results);
  stream.handshake(ssl::stream_base::client);
  http::request<http::string_body> req{http::verb::get, target, version};
  req.set(http::field::host, host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  beast::flat_buffer buffer;
  http::response<http::dynamic_body> res;
  http::read(stream, buffer, res);

  beast::error_code ec;
  stream.shutdown(ec);
  if (ec == net::error::eof)
    ec = {};
  if (ec)
    throw beast::system_error{ec};
  const auto end = mc::local_time();

  return {
      .uri = host,
      .elapsed_time = (end - start).total_milliseconds(),
      .response_size = 0L,
  };
}

int main() {
  auto const host = "github.com";
  auto const port = "443";
  auto const target = "/robots.txt";

  benchmark m;
  cout << m.one_run(host, port, target) << endl;

  return 0;
}
