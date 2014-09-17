//  ===== ---- ofp/deferred.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the Deferred class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DEFERRED_H_
#define OFP_DEFERRED_H_

#include <memory>

namespace ofp {
namespace detail {

OFP_BEGIN_IGNORE_PADDING

template <class Type>
class DeferredResult
    : public std::enable_shared_from_this<DeferredResult<Type>> {
public:
  using SharedPtr = std::shared_ptr<DeferredResult<Type>>;
  using Callback = std::function<void(Type)>;

  void done(const Type &result) {
    if (isDone)
      return;
    isDone = true;
    if (callback_ != nullptr) {
      if (!isCalled) {
        isCalled = true;
        callback_(result);
      }
    } else {
      // Callback may not have been set yet. Save value.
      result_ = result;
    }
  }

  void setCallback(const Callback &callback) {
    callback_ = callback;
    if (isDone && callback_ != nullptr && !isCalled) {
      isCalled = true;
      callback_(result_);
    }
  }

private:
  Callback callback_{nullptr};
  Type result_{};
  bool isDone = false;
  bool isCalled = false;
};
OFP_END_IGNORE_PADDING

template <class Type>
using DeferredResultCallback = typename DeferredResult<Type>::Callback;

}  // namespace detail

template <class Type>
using DeferredResultPtr = typename detail::DeferredResult<Type>::SharedPtr;

template <class Type>
class Deferred {
public:
  explicit Deferred() {}

  /* implicit NOLINT */ Deferred(const DeferredResultPtr<Type> &result)
      : result_{result} {}
  /* implicit NOLINT */ Deferred(const Type &result) : result_{makeResult()} {
    result_->done(result);
  }

  void done(const detail::DeferredResultCallback<Type> &callback) {
    result_->setCallback(callback);
  }

  static DeferredResultPtr<Type> makeResult() {
    return std::make_shared<detail::DeferredResult<Type>>();
  }

private:
  DeferredResultPtr<Type> result_;
};

}  // namespace ofp

#endif  // OFP_DEFERRED_H_
