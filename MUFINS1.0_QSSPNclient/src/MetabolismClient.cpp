#include "MetabolismClient.h"

#include <cassert>

namespace PNFBA {

MetabolismClient::MetabolismClient(const std::string& host, 
                                   const std::string& port) :
  io_service_(), resolver_(io_service_), 
  query_(asio::ip::tcp::v4(), host, port), 
  socket_(io_service_) {
  iterator_ = resolver_.resolve(query_);
  asio::error_code error;

  socket_.connect(*iterator_, error);
  if (error) {
    std::cerr << "ERROR: cannot connect to Metabolism server!\n";
    std::exit(1);
  }
}
 
MetabolismClient::~MetabolismClient() {
  char c;
  asio::write(socket_, asio::buffer(&(c), 0));
  asio::error_code ec;
  socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
  socket_.close();
}

double MetabolismClient::runMetabolism(const std::string& input) {
  asio::write(socket_, asio::buffer(input.c_str(), input.length() + 1));
  
  double reply;
  size_t reply_length = asio::read(socket_, 
                                   asio::buffer(&reply, sizeof(double)));
  assert(reply_length == sizeof(double));
  return reply;
}

}  // namespace PNFBA