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
      case TableFeaturePropertyInstructionsMiss::type() :
        if (!prop.property<TableFeaturePropertyInstructions>().validateInput())
          return false;
        break;

      case TableFeaturePropertyNextTables::type() :
      case TableFeaturePropertyNextTablesMiss::type() :
        if (!prop.property<TableFeaturePropertyNextTables>().validateInput())
          return false;
        break;

      case TableFeaturePropertyWriteActions::type() :
      case TableFeaturePropertyWriteActionsMiss::type() :
      case TableFeaturePropertyApplyActions::type() :
      case TableFeaturePropertyApplyActionsMiss::type() :
        if (!prop.property<TableFeaturePropertyWriteActions>().validateInput())
          return false;
        break;

      case TableFeaturePropertyMatch::type() :
      case TableFeaturePropertyWildcards::type() :
      case TableFeaturePropertyWriteSetField::type() :
      case TableFeaturePropertyWriteSetFieldMiss::type() :
      case TableFeaturePropertyApplySetField::type() :
      case TableFeaturePropertyApplySetFieldMiss::type() :
        if (!prop.property<TableFeaturePropertyMatch>().validateInput())
          return false;
        break;
    }
  }

  return true;
}
