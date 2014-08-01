#include "ofp/mpmeterfeatures.h"
#include "ofp/validation.h"

using namespace ofp;

bool MPMeterFeatures::validateInput(Validation *context) const {
    size_t length = context->lengthRemaining();
    if (length != sizeof(MPMeterFeatures)) {
        context->lengthRemainingIsInvalid(BytePtr(this), sizeof(MPMeterFeatures));
        return false;
    }
    return true;
}
