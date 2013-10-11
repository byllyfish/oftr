#ifndef OFP_YAML_YBUCKET_H
#define OFP_YAML_YBUCKET_H

#include "ofp/bucket.h"
#include "ofp/bucketlist.h"
#include "ofp/bucketiterator.h"

namespace ofp { // <namespace ofp>
namespace detail { // <namespace detail>

struct BucketInserter 
{
};

} // </namespace detail>
} // </namespace ofp>

namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>


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

        UInt16 weight = bucket.weight();
        UInt32 watchPort = bucket.watchPort();
        UInt32 watchGroup = bucket.watchGroup();
        io.mapRequired("weight", weight);
        io.mapRequired("watch_port", watchPort);
        io.mapRequired("watch_group", watchGroup);

        ActionRange actions = bucket.actions();
        io.mapRequired("actions", actions);
    }
};


template <>
struct SequenceTraits<ofp::BucketRange> {

    static size_t size(IO &io, ofp::BucketRange &buckets)
    {
        return buckets.itemCount();
    }

    static ofp::BucketIterator::Item &element(IO &io, ofp::BucketRange &buckets,
                                     size_t index)
    {
        ofp::log::debug("bucket yaml item", index);
        // FIXME
        ofp::BucketIterator iter = buckets.begin();
        for (size_t i = 0; i < index; ++i)
            ++iter;
        return RemoveConst_cast(*iter);
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


} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YBUCKET_H
