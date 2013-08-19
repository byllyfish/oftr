#ifndef OFP_YAML_BYTEORDER_H
#define OFP_YAML_BYTEORDER_H

#include "ofp/yaml/llvm_yaml.h"
#include "ofp/byteorder.h"


template <>
struct llvm::yaml::ScalarTraits<ofp::Big16> {
    static void output(const ofp::Big16 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        ScalarTraits<uint16_t>::output(value, ctxt, out);
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::Big16 &value)
    {
        uint16_t num = 0;
        auto err = ScalarTraits<uint16_t>::input(scalar, ctxt, num);
        if (err.empty()) {
            value = num;
        }
        return err;
    }
};


template <>
struct llvm::yaml::ScalarTraits<ofp::Big32> {
    static void output(const ofp::Big32 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        ScalarTraits<uint32_t>::output(value, ctxt, out);
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::Big32 &value)
    {
        uint32_t num = 0;
        auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
        if (err.empty()) {
            value = num;
        }
        return err;
    }
};


template <>
struct llvm::yaml::ScalarTraits<ofp::Big64> {
    static void output(const ofp::Big64 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        ScalarTraits<uint64_t>::output(value, ctxt, out);
    }

    static StringRef input(StringRef scalar, void *ctxt, ofp::Big64 &value)
    {
        uint64_t num = 0;
        auto err = ScalarTraits<uint64_t>::input(scalar, ctxt, num);
        if (err.empty()) {
            value = num;
        }
        return err;
    }
};

#endif // OFP_YAML_BYTEORDER_H
