#include "ofp/tablefeatureproperty.h"
#include "ofp/propertyrange.h"

using namespace ofp;

bool TableFeaturePropertyValidator::validateInput(const PropertyRange &range) {
  if (!range.validateInput("TableFeatureProperty")) {
    return false;
  }

  for (auto &prop : range) {
    switch (prop.type()) {
      case TableFeaturePropertyInstructions::type() :
        if (!prop.property<TableFeaturePropertyInstructions>().validateInput())
          return false;
        break;

      case TableFeaturePropertyWriteActions::type() :
      case TableFeaturePropertyWriteActionsMiss::type() :
      case TableFeaturePropertyApplyActions::type() :
      case TableFeaturePropertyApplyActionsMiss::type() :
        if (!prop.property<TableFeaturePropertyWriteActions>().validateInput())
          return false;
        break;
    }
  }

  return true;
}
