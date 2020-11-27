#include <boost/asio/strand.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "certificates.hpp"

namespace beast = boost::beast;    // from <boost/beast.hpp>
namespace http = beast::http;      // from <boost/beast/http.hpp>
namespace net = boost::asio;       // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;  // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

using namespace boost;

int main() {
  auto const host = "github.com";
  auto const port = "443";
  auto const target = "/robots.txt";
  auto const version = 11;

  net::io_context ioc;
  // The SSL context is required, and holds certificates
  ssl::context ctx{ssl::context::tlsv12_client};
  load_root_certificates(ctx);
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
  std::cout << res << std::endl;

  // Gracefully close the stream
  beast::error_code ec;
  stream.shutdown(ec);
  if (ec == net::error::eof) {
    ec = {};
  }
  if (ec) throw beast::system_error{ec};

  return 0;
}
