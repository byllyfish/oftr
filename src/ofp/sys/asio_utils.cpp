// Copyright 2014-present Bill Fisher. All rights reserved.

#include <system_error>
#include "ofp/sys/asio_utils.h"
#include "ofp/log.h"

// BoringSSL fix.
void ERR_remove_state(unsigned long pid) {
    ::ERR_remove_thread_state((const CRYPTO_THREADID *)pid);
}

#if ASIO_NO_EXCEPTIONS

namespace asio {
namespace detail {

template <typename Exception>
void throw_exception(const Exception &e) {
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

template void throw_exception(const asio::invalid_service_owner &);
template void throw_exception(const asio::service_already_exists &);
template void throw_exception(const asio::ip::bad_address_cast &);

}  // namespace detail
}  // namespace asio

#endif  // ASIO_NO_EXCEPTIONS
