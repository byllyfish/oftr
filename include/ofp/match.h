//  ===== ---- ofp/match.h ---------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Match class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_MATCH_H
#define OFP_MATCH_H

#include "ofp/standardmatch.h"

namespace ofp { // <namespace ofp>

class Match {
public:
    explicit Match(OXMRange range) : oxm_{range}
    {
    	log::debug("Match created:", range);
    }

    explicit Match(const deprecated::StandardMatch *match)
        : oxm_{match->toOXMList()}
    {
    }

    /// \returns number of items in the match.
    size_t itemCount() const
    {
        return OXMIterator::distance(oxm_.begin(), oxm_.end());
    }

    OXMIterator begin() const
    {
        return oxm_.begin();
    }
    OXMIterator end() const
    {
        return oxm_.end();
    }

private:
    OXMList oxm_;
};

} // </namespace ofp>

#endif // OFP_MATCH_H
