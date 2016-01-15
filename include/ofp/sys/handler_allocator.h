// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SYS_HANDLER_ALLOCATOR_H_
#define OFP_SYS_HANDLER_ALLOCATOR_H_

namespace ofp {
namespace sys {

// [Adapted from server.cpp]
//
// Class to manage the memory to be used for handler-based custom allocation.
// It contains a single block of memory which may be returned for allocation
// requests. If the memory is in use when an allocation request is made, the
// allocator delegates allocation to the global heap.
//
// The last byte of the memory block is reserved for the flag that indicates
// if the intrinsic memory block is in use.
class handler_allocator {
 public:
  enum : size_t { IntrinsicSize = 183 };

  handler_allocator() { set_in_use(false); }

  handler_allocator(const handler_allocator &) = delete;
  handler_allocator &operator=(const handler_allocator &) = delete;

  void *allocate(std::size_t size) {
    if (!in_use() && size <= IntrinsicSize) {
      set_in_use(true);
      return storage_;
    } else {
      log::debug("handler_allocator::allocate: intrinsic block in use:", size);
      return ::operator new(size);
    }
  }

  void deallocate(void *pointer) {
    if (pointer == storage_) {
      set_in_use(false);
    } else {
      ::operator delete(pointer);
    }
  }

 private:
  // Storage space used for handler-based custom memory allocation. The last
  // byte is used for the in_use flag; the size of this object is aligned to
  // 8 bytes.
  OFP_ALIGNAS(8) UInt8 storage_[IntrinsicSize + 1];

  bool in_use() const { return storage_[IntrinsicSize]; }
  void set_in_use(bool val) { storage_[IntrinsicSize] = val; }
};

static_assert(sizeof(handler_allocator) % 8 == 0, "Expected aligned size.");

// Wrapper class template for handler objects to allow handler memory
// allocation to be customised. Calls to operator() are forwarded to the
// encapsulated handler.
template <typename Handler>
class custom_alloc_handler {
 public:
  custom_alloc_handler(handler_allocator &a, Handler h)
      : allocator_(a), handler_(h) {}

  template <typename... Args>
  void operator()(Args &&... args) {
    handler_(std::forward<Args>(args)...);
  }

  friend void *asio_handler_allocate(
      std::size_t size, custom_alloc_handler<Handler> *this_handler) {
    return this_handler->allocator_.allocate(size);
  }

  friend void asio_handler_deallocate(
      void *pointer, std::size_t /*size*/,
      custom_alloc_handler<Handler> *this_handler) {
    this_handler->allocator_.deallocate(pointer);
  }

 private:
  handler_allocator &allocator_;
  Handler handler_;
};

// Helper function to wrap a handler object to add custom allocation.
template <typename Handler>
inline custom_alloc_handler<Handler> make_custom_alloc_handler(
    handler_allocator &a, Handler h) {
  return custom_alloc_handler<Handler>(a, h);
}

}  // namespace sys
}  // namespace ofp

#endif  // OFP_SYS_HANDLER_ALLOCATOR_H_
