// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/tablemodproperty.h"
#include "ofp/propertyrange.h"

using namespace ofp;

bool TableModPropertyValidator::validateInput(const PropertyRange &range,
                                              Validation *context) {
  for (const auto &prop : range) {
    switch (prop.type()) {
      case TableModPropertyEviction::type():
        if (!prop.property<TableModPropertyEviction>().validateInput(context))
          return false;
        break;
      case TableModPropertyVacancy::type():
        if (!prop.property<TableModPropertyVacancy>().validateInput(context))
          return false;
        break;
      case TableModPropertyExperimenter::type():
        if (!prop.property<TableModPropertyExperimenter>().validateInput(
                context))
          return false;
        break;
    }
  }

  return true;
}
