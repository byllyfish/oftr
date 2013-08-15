#ifndef OFP_DEFERRED_H
#define OFP_DEFERRED_H

#include "ofp/log.h"
#include <memory>

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

OFP_BEGIN_IGNORE_PADDING
template <class Type>
class DeferredResult : public std::enable_shared_from_this<DeferredResult<Type>> {
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
	log::Lifetime lifetime_{"DeferredResult"};
};
OFP_END_IGNORE_PADDING

template <class Type>
using DeferredResultCallback = typename DeferredResult<Type>::Callback;

} // </namespace detail>


template <class Type>
using DeferredResultPtr = typename detail::DeferredResult<Type>::SharedPtr;


template <class Type>
class Deferred {
public:

	/* implicit */ Deferred(const DeferredResultPtr<Type> &result) : result_{result} {}

	void done(const detail::DeferredResultCallback<Type> &callback) {
		result_->setCallback(callback);
	}

	static DeferredResultPtr<Type> makeResult() {
		return std::make_shared<detail::DeferredResult<Type>>();
	}

private:
	DeferredResultPtr<Type> result_;
	log::Lifetime lifetime_{"Deferred"};
};

} // </namespace ofp>

#endif // OFP_DEFERRED_H
