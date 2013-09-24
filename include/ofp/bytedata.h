//  ===== ---- ofp/bytedata.h ------------------------------*- C++ -*- =====  //
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
/// \brief Defines the ByteData class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_BYTEDATA_H
#define OFP_BYTEDATA_H

#include "ofp/byterange.h"
#include <memory>

namespace ofp { // <namespace ofp>

class ByteData {
public:
    ByteData();
    ByteData(const void *data, size_t length);
    explicit ByteData(const ByteRange &range);
    ByteData(ByteData &&data);

    //ByteData(ByteData &&) = default;

    const UInt8 *data() const;
    size_t size() const;

    ByteRange toRange() const;

private:
    std::unique_ptr<UInt8[]> buf_;
    size_t size_;
};

} // </namespace ofp>

inline ofp::ByteData::ByteData() : buf_{nullptr}, size_{0}
{
}

inline ofp::ByteData::ByteData(const void *data, size_t length)
    : buf_{new UInt8[length]}, size_{length}
{
    std::memcpy(buf_.get(), data, length);
}

inline ofp::ByteData::ByteData(const ByteRange &range)
    : ByteData{range.data(), range.size()}
{
}

inline ofp::ByteData::ByteData(ByteData &&other) : buf_{std::move(other.buf_)}, size_{other.size_}
{
	other.size_ = 0;
}

inline const ofp::UInt8 *ofp::ByteData::data() const
{
    return buf_.get();
}

inline size_t ofp::ByteData::size() const
{
    return size_;
}

inline ofp::ByteRange ofp::ByteData::toRange() const
{
	return ByteRange{data(), size()};
}


#endif // OFP_BYTEDATA_H
