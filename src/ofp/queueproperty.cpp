#include "ofp/queueproperty.h"
#include "ofp/propertyrange.h"

using namespace ofp;

bool QueuePropertyExperimenter::validateInput(Validation *context) const {
    return len_ >= FixedHeaderSize;
}

bool QueuePropertyValidator::validateInput(const PropertyRange &range, Validation *context) {
  for (const auto &prop : range) {
    switch (prop.type()) {
        case QueuePropertyExperimenter::type(): 
            if (!prop.property<QueuePropertyExperimenter>().validateInput(context))
                return false;
            break;
    }
  }

  return true;
}
