#include "ofp/mpgroupfeatures.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPGroupFeatures::validateInput(Validation *context) const {
    size_t length = context->lengthRemaining();
    if (length != sizeof(MPGroupFeatures)) {
        context->lengthRemainingIsInvalid(BytePtr(this), sizeof(MPGroupFeatures));
        return false;
    }
    return true;
}
