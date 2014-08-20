//  ===== ---- ofp/sys/asio_utils.cpp ----------------------*- C++ -*- =====  //
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

#include "ofp/sys/asio_utils.h"
#include "ofp/log.h"
#include <system_error>

#if ASIO_NO_EXCEPTIONS

namespace asio {
namespace detail {

template <typename Exception>
void throw_exception(const Exception& e)
{
  // With ASIO_NO_EXPCEPTIONS defined, we need to define our own version of
  // asio::detail::throw_exception. GCC requires ASIO_NO_EXCEPTIONS to compile
  // with -fno-exceptions. (Clang does not.)

  ofp::log::fatal("asio::detail::throw_exception:", e.what());
  std::terminate();
}

template void throw_exception(const std::system_error &);
template void throw_exception(const std::bad_cast &);
template void throw_exception(const std::out_of_range &);
template void throw_exception(const std::length_error &);

} // namespace detail
} // namespace asio

#endif //ASIO_NO_EXCEPTIONS

