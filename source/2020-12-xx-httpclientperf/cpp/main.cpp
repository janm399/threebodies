#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "certificates.hpp"

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

using namespace std;  // so shoot me

vector<string> getUris() {
  ifstream uri_file("../uris.txt");
  vector<string> result;
  copy(istream_iterator<string>(uri_file), istream_iterator<string>(),
       back_inserter(result));
  return result;
}

struct request_benchmark {
  string uri;
  long elapsed_time;
  long response_size;
};

struct run_requests {
  vector<request_benchmark> responses;
  long elapsed_time;

  friend ostream& operator<<(ostream& os, const run_requests& p) {
    sort(begin(p.responses), end(p.responses),
         [](auto const& a, auto const& b) { return a.uri < b.uri; });

    return os;
  }
};

int main() {
  auto const host = "github.com";
  auto const port = "443";
  auto const target = "/robots.txt";
  auto const version = 11;  // HTTP version 1.1

  net::io_context ioc;
  // The SSL context is required, and holds certificates
  ssl::context ctx{ssl::context::tlsv12_client};
  ctx.load_verify_file("/etc/ssl/cert.pem");
  ctx.set_verify_mode(ssl::verify_peer);
  tcp::resolver resolver(ioc);
  beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
  if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
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

  // Send the HTTP request to the remote host
  http::write(stream, req);

  // This buffer is used for reading and must be persisted
  beast::flat_buffer buffer;

  // Declare a container to hold the response
  http::response<http::dynamic_body> res;

  // Receive the HTTP response
  http::read(stream, buffer, res);

  // Write the message to standard out
  cout << res << endl;

  // Gracefully close the stream
  beast::error_code ec;
  stream.shutdown(ec);
  if (ec == net::error::eof) {
    ec = {};
  }
  if (ec) throw beast::system_error{ec};

  return 0;
}
