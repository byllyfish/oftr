//  ===== ---- ofp/yaml/yaddress.h -------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the llvm::yaml::ScalarTraits for IPv4Address, IPv6Address,
/// and EnetAddress.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_YADDRESS_H_
#define OFP_YAML_YADDRESS_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/enetaddress.h"
#include "ofp/ipv6endpoint.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::IPv4Address> {
    static void output(const ofp::IPv4Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv4Address &value)
    {
        if (scalar.empty()) {
            value.clear();
            return "";
        }

        if (!value.parse(scalar)) {
            return "Invalid IPv4 address.";
        }
        return "";
    }
};

template <>
struct ScalarTraits<ofp::IPv6Address> {
    static void output(const ofp::IPv6Address &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv6Address &value)
    {
        if (scalar.empty()) {
            value.clear();
            return "";
        }

        if (!value.parse(scalar)) {
            return "Invalid IPv6 address.";
        }
        return "";
    }
};

template <>
struct ScalarTraits<ofp::EnetAddress> {
    static void output(const ofp::EnetAddress &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::EnetAddress &value)
    {
        if (scalar.empty()) {
            value.clear();
            return "";
        }
        
        if (!value.parse(scalar)) {
            return "Invalid Ethernet address.";
        }
        return "";
    }
};

template <>
struct ScalarTraits<ofp::IPv6Endpoint> {
    static void output(const ofp::IPv6Endpoint &value, void *ctxt,
                       llvm::raw_ostream &out)
    {
        out << value.toString();
    }

    static StringRef input(StringRef scalar, void *ctxt,
                           ofp::IPv6Endpoint &value)
    {
        if (scalar.empty()) {
            value.clear();
            return "";
        }

        if (!value.parse(scalar)) {
            return "Invalid IPv6 endpoint.";
        }
        return "";
    }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YADDRESS_H_
