//  ===== ---- ofp/actionlist.h ----------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the ActionList class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONLIST_H
#define OFP_ACTIONLIST_H

#include "ofp/types.h"
#include "ofp/bytelist.h"
#include "ofp/actionrange.h"

namespace ofp { // <namespace ofp>

class ActionList {
  public:

    ActionList() = default;

    const UInt8 *data() const
    {
        return buf_.data();
    }
    
    size_t size() const
    {
        return buf_.size();
    }

    template <class Type> void add(const Type &action)
    {
        // Make sure it's an action.
        assert(Type::type() != 0);
        buf_.add(&action, sizeof(action));
    }

    ActionRange toRange() const { return ActionRange{buf_.toRange()}; }

  private:
    ByteList buf_;
};

} // </namespace ofp>

#endif // OFP_ACTIONLIST_H
