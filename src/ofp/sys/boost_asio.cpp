//  ===== ---- ofp/sys/boost_asio.cpp ----------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Implements Boost ASIO utility functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/sys/boost_asio.h"
#include <system_error>

size_t ofp::sys::HashEndpoint::operator()(const IPv6Endpoint &endpt) const {
  // FIXME - check literature for better hash function.

  // Hash the endpoint starting with the port and the least signficant bytes
  // of the IP address.

  const IPv6Address &addr = endpt.address();
  UInt16 port = endpt.port();

  auto data = addr.toArray();
  UInt32 *quad = reinterpret_cast<UInt32 *>(&data[0]) + 3;
  UInt64 sum = *quad * (port + 37);
  for (int i = 0; i < 3; ++i)
    sum += 41 * sum + (*quad-- + 37);
  sum += 41 * sum + (*quad + 37);
  return sum;
}

namespace asio {
namespace detail {

template <typename Exception>
void throw_exception(const Exception& e)
{
  std::terminate();
}

template void throw_exception(const std::system_error &);
template void throw_exception(const std::bad_cast &);
template void throw_exception(const std::out_of_range &);
template void throw_exception(const std::length_error &);

} // namespace detail
} // namespace asio

