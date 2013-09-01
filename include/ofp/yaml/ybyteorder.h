#ifndef OFP_YAML_YBYTEORDER_H
#define OFP_YAML_YBYTEORDER_H

#include "ofp/yaml/yllvm.h"
#include "ofp/byteorder.h"


namespace llvm { // <namespace llvm>
namespace yaml { // <namespace yaml>


template <>
struct ScalarTraits<ofp::Big16> {
    static void output(const ofp::Big16 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        // Output Big16 in hexadecimal.
        uint16_t num = value;
        ScalarTraits<Hex16>::output(num, ctxt, out);
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
struct ScalarTraits<ofp::Big32> {
    static void output(const ofp::Big32 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        // Output Big32 in hexadecimal.
        uint32_t num = value;
        ScalarTraits<Hex32>::output(num, ctxt, out);
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
struct ScalarTraits<ofp::Big64> {
    static void output(const ofp::Big64 &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        // Output Big64 in hexadecimal.
        uint64_t num = value;
        ScalarTraits<Hex64>::output(num, ctxt, out);
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


} // </namespace yaml>
} // </namespace llvm>

#endif // OFP_YAML_YBYTEORDER_H
