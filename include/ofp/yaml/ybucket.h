//  ===== ---- ofp/ybucket.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the llvm::yaml::MappingTraits for Bucket and the
/// llvm::yaml::SequenceTraits for BucketList and BucketRange.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YBUCKET_H_
#define OFP_YAML_YBUCKET_H_

#include "ofp/bucket.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketrange.h"

namespace ofp {
namespace detail {

struct BucketInserter 
{
};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {


template <>
struct MappingTraits<ofp::detail::BucketInserter> {

    static void mapping(IO &io, ofp::detail::BucketInserter &inserter)
    {
        using namespace ofp;

        BucketList &buckets = reinterpret_cast<BucketList&>(inserter);

        UInt16 weight;
        UInt32 watchPort;
        UInt32 watchGroup;
        io.mapRequired("weight", weight);
        io.mapRequired("watch_port", watchPort);
        io.mapRequired("watch_group", watchGroup);

        ActionList actions;
        io.mapRequired("actions", actions);

        BucketBuilder bkt;
        bkt.setWeight(weight);
        bkt.setWatchPort(watchPort);
        bkt.setWatchGroup(watchGroup);
        bkt.setActions(actions);
        buckets.add(bkt);
    }
};


template <>
struct MappingTraits<ofp::Bucket> {

    static void mapping(IO &io, ofp::Bucket &bucket)
    {
        using namespace ofp;

        Hex16 weight = bucket.weight();
        Hex32 watchPort = bucket.watchPort();
        Hex32 watchGroup = bucket.watchGroup();
        io.mapRequired("weight", weight);
        io.mapRequired("watch_port", watchPort);
        io.mapRequired("watch_group", watchGroup);

        ActionRange actions = bucket.actions();
        io.mapRequired("actions", actions);
    }
};


template <>
struct SequenceTraits<ofp::BucketRange> {
    using iterator = ofp::BucketIterator;

    static iterator begin(IO &io, ofp::BucketRange &range) {
        return range.begin();
    }

    static iterator end(IO &io, ofp::BucketRange &range) {
        return range.end();
    }

    static void next(iterator &iter, iterator iterEnd) {
        ++iter;
    }
};


template <>
struct SequenceTraits<ofp::BucketList> {

    static size_t size(IO &io, ofp::BucketList &list)
    {
        return 0;
    }

    static ofp::detail::BucketInserter &element(IO &io, ofp::BucketList &list,
                                     size_t index)
    {
        return reinterpret_cast<ofp::detail::BucketInserter &>(list);
    }
};


}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUCKET_H_
