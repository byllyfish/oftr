#ifndef OFP_EXPERIMENTER_H
#define OFP_EXPERIMENTER_H

#include "ofp/header.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class Message;
class Writable;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E x p e r i m e n t e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements immutable Experimenter protocol message.

class Experimenter {
public:

    static constexpr OFPType type()
    {
        return OFPT_EXPERIMENTER;
    }

    static const Experimenter *cast(const Message *message);

    Experimenter() : header_{type()}
    {
    }

    UInt32 experimenter() const
    {
        return experimenter_;
    }
    
    UInt32 expType() const
    {
        return expType_;
    }

    ByteRange expData() const;

    bool validateLength(size_t length) const;

private:
    Header header_;
    Big32 experimenter_;
    Big32 expType_;

    friend class ExperimenterBuilder;
};

static_assert(sizeof(Experimenter) == 16, "Unexpected size.");
static_assert(IsStandardLayout<Experimenter>(), "Expected standard layout.");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E x p e r i m e n t e r B u i l d e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements Experimenter protocol message builder.

class ExperimenterBuilder {
public:

    ExperimenterBuilder() {}

    void setExperimenter(UInt32 experimenter)
    {
        msg_.experimenter_ = experimenter;
    }

    void setExpType(UInt32 expType)
    {
        msg_.expType_ = expType;
    }

    void setExpData(const void *data, size_t length) {
        data_.set(data, length);
    }

    UInt32 send(Writable *channel);

private:
    Experimenter msg_;
    ByteList data_;
};

} // </namespace ofp>

#endif // OFP_EXPERIMENTER_H
