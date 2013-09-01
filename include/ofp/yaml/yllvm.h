#ifndef OFP_YAML_YLLVM_H
#define OFP_YAML_YLLVM_H

// The following two definitions are required by llvm/Support/DataTypes.h
#define __STDC_LIMIT_MACROS 1
#define __STDC_CONSTANT_MACROS 1

#include "llvm/Support/YAMLTraits.h"
#include "llvm/Support/YAMLParser.h"


template <class Type>
Type &YamlRemoveConst_cast(const Type &v)
{
    return const_cast<Type &>(v);
}


#endif // OFP_YAML_YLLVM_H
