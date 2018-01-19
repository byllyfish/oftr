// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/sys/asio_utils.h"
#include "ofp/unittest.h"

TEST(asio, async_connect_v4) {
  auto localhost_v4 = asio::ip::address_v4::loopback();

  asio::error_code result;
  asio::io_context io;
  asio::ip::tcp::socket socket{io};

  EXPECT_FALSE(socket.is_open());

  socket.async_connect(
      {localhost_v4, 9},
      [&result](const asio::error_code &error) { result = error; });

  EXPECT_TRUE(socket.is_open());

  io.run();

  EXPECT_TRUE(socket.is_open());
  EXPECT_EQ(asio::error::connection_refused, result);

  socket.close();
  EXPECT_FALSE(socket.is_open());
}

static bool isConnectionRefusedOrNetworkUnreachable(asio::error_code &err) {
  // (2017-08-21) On Alpine Linux 3.6.2 the result can also be EADDRNOTAVAIL
  // when IPv6 loopback not enabled.
  return (err == asio::error::connection_refused) ||
         (err == asio::error::network_unreachable) ||
         (err == std::errc::address_not_available);
}

TEST(asio, async_connect_v6) {
  auto localhost_v6 = asio::ip::address_v6::loopback();

  asio::error_code result;
  asio::io_context io;
  asio::ip::tcp::socket socket{io};

  socket.async_connect(
      {localhost_v6, 9},
      [&result](const asio::error_code &error) { result = error; });

  io.run();

  EXPECT_TRUE(socket.is_open());

  // A network connection to ::1 may be unreachable because IPv6 is disabled on
  // the loopback interface.
  EXPECT_TRUE(isConnectionRefusedOrNetworkUnreachable(result) || result);
  std::cerr << "async_connect_v6: " << result << '\n';

  socket.close();
  EXPECT_FALSE(socket.is_open());
}

TEST(asio, chrono_time_traits) {
  // Make sure that there is no overflow in the implementation of duration_cast
  // in asio::detail::chrono_time_traits.
  //
  // This test was written on Mac OS X; it may need to be revised for systems
  // where the steady_clock has less than nanosecond precision.

  using Clock = std::chrono::steady_clock;
  using TimeTraits =
      asio::detail::chrono_time_traits<Clock, asio::wait_traits<Clock>>;

  const int64_t value = 9223231944883008317LL;
  std::chrono::nanoseconds nano{value};

  TimeTraits::posix_time_duration test1{nano};
  EXPECT_EQ(value, test1.ticks());

  EXPECT_EQ(value / 1000000000, test1.total_seconds());
  EXPECT_EQ(value / 1000000, test1.total_milliseconds());
  EXPECT_EQ(value / 1000, test1.total_microseconds());
}
