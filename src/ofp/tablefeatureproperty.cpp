#include "ofp/tablefeatureproperty.h"
#include "ofp/propertyrange.h"

using namespace ofp;

bool TableFeaturePropertyValidator::validateInput(const PropertyRange &range, Validation *context) {
  for (auto &prop : range) {
    switch (prop.type()) {
      case TableFeaturePropertyInstructions::type() :
      case TableFeaturePropertyInstructionsMiss::type() :
        if (!prop.property<TableFeaturePropertyInstructions>().validateInput(context))
          return false;
        break;

      case TableFeaturePropertyNextTables::type() :
      case TableFeaturePropertyNextTablesMiss::type() :
        if (!prop.property<TableFeaturePropertyNextTables>().validateInput(context))
          return false;
        break;

      case TableFeaturePropertyWriteActions::type() :
      case TableFeaturePropertyWriteActionsMiss::type() :
      case TableFeaturePropertyApplyActions::type() :
      case TableFeaturePropertyApplyActionsMiss::type() :
        if (!prop.property<TableFeaturePropertyWriteActions>().validateInput(context))
          return false;
        break;

      case TableFeaturePropertyMatch::type() :
      case TableFeaturePropertyWildcards::type() :
      case TableFeaturePropertyWriteSetField::type() :
      case TableFeaturePropertyWriteSetFieldMiss::type() :
      case TableFeaturePropertyApplySetField::type() :
      case TableFeaturePropertyApplySetFieldMiss::type() :
        if (!prop.property<TableFeaturePropertyMatch>().validateInput(context))
          return false;
        break;
    }
  }

  return true;
}
