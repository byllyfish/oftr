#include "ofp/unittest.h"
#include "ofp/sys/boost_asio.h"

TEST(asio, async_connect_v4) {
  auto localhost_v4 = asio::ip::address_v4::loopback();

  asio::error_code result;
  asio::io_service service;
  asio::ip::tcp::socket socket{service};

  EXPECT_FALSE(socket.is_open());

  socket.async_connect(
      {localhost_v4, 9},
      [&result](const asio::error_code &error) { result = error; });

  EXPECT_TRUE(socket.is_open());

  service.run();

  EXPECT_TRUE(socket.is_open());
  EXPECT_EQ(asio::error::connection_refused, result);

  socket.close();
  EXPECT_FALSE(socket.is_open());
}

TEST(asio, async_connect_v6) {
  auto localhost_v6 = asio::ip::address_v6::loopback();

  asio::error_code result;
  asio::io_service service;
  asio::ip::tcp::socket socket{service};

  socket.async_connect(
      {localhost_v6, 9},
      [&result](const asio::error_code &error) { result = error; });

  service.run();

  EXPECT_EQ(asio::error::connection_refused, result);
}
