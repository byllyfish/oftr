// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YPORTMOD_H_
#define OFP_YAML_YPORTMOD_H_

#include "ofp/portmod.h"
#include "ofp/yaml/yportmodproperty.h"

namespace llvm {
namespace yaml {

const char *const kPortModSchema = R"""({Message/PortMod}
type: 'PORT_MOD'
msg:
  port_no: PortNumber
  hw_addr: EnetAddress
  config: PortConfigFlags
  mask: PortConfigFlags
  advertise: PortFeaturesFlags
)""";

template <>
struct MappingTraits<ofp::PortMod> {
  static void mapping(IO &io, ofp::PortMod &msg) {
    io.mapRequired("port_no", msg.portNo_);
    io.mapRequired("hw_addr", msg.hwAddr_);

    ofp::OFPPortConfigFlags config = msg.config();
    ofp::OFPPortConfigFlags mask = msg.mask();
    io.mapRequired("config", config);
    io.mapRequired("mask", mask);

    ofp::PropertyRange props = msg.properties();

    auto eprop = props.findProperty(ofp::PortModPropertyEthernet::type());
    if (eprop != props.end()) {
      const ofp::PortModPropertyEthernet &eth = eprop->property<ofp::PortModPropertyEthernet>();
      io.mapRequired("ethernet", RemoveConst_cast(eth));
    }

    auto oprop = props.findProperty(ofp::PortModPropertyOptical::type());
    if (oprop != props.end()) {
      const ofp::PortModPropertyOptical &opt = oprop->property<ofp::PortModPropertyOptical>();
      io.mapRequired("optical", RemoveConst_cast(opt));
    }

    io.mapRequired("properties", Ref_cast<ofp::detail::PortModPropertyRange>(props));
  }
};

template <>
struct MappingTraits<ofp::PortModBuilder> {
  static void mapping(IO &io, ofp::PortModBuilder &msg) {
    io.mapRequired("port_no", msg.msg_.portNo_);
    io.mapRequired("hw_addr", msg.msg_.hwAddr_);

    ofp::OFPPortConfigFlags config = ofp::OFPPC_NONE;
    ofp::OFPPortConfigFlags mask = ofp::OFPPC_NONE;
    io.mapRequired("config", config);
    io.mapRequired("mask", mask);
    msg.setConfig(config);
    msg.setMask(mask);

    ofp::PortModPropertyEthernet eth;
    io.mapRequired("ethernet", eth);   // FIXME(bfish) - make optional?

    Optional<ofp::PortModPropertyOptical> opt;
    io.mapOptional("optical", opt);

    ofp::PropertyList props;
    props.add(eth);
    if (opt) {
        props.add(*opt);
    }

    io.mapRequired("properties", Ref_cast<ofp::detail::PortModPropertyList>(props));
    msg.setProperties(props);
  }
};


template <>
struct MappingTraits<ofp::detail::PortModPropertyItem> {
  static void mapping(IO &io, ofp::detail::PortModPropertyItem &item) {
    using namespace ofp;

    PropertyIterator::Element &elem = Ref_cast<PropertyIterator::Element>(item);
    Hex16 property = elem.type();
    io.mapRequired("property", property);

    switch (property) {
      case OFPPDPT_EXPERIMENTER: {
        const PortModPropertyExperimenter &p =
            elem.property<PortModPropertyExperimenter>();

        Hex32 experimenter = p.experimenter();
        io.mapRequired("experimenter", experimenter);

        Hex32 expType = p.expType();
        io.mapRequired("exp_type", expType);

        ByteRange expData = p.expData();
        io.mapRequired("data", expData);
        break;
      }
      default: {
        log::debug("Unsupported PortModPropertyItem");
        ByteRange data = elem.value();
        io.mapRequired("data", data);
        break;
      }
    }
  }
};

template <>
struct MappingTraits<ofp::detail::PortModPropertyInserter> {
  static void mapping(IO &io, ofp::detail::PortModPropertyInserter &list) {
    using namespace ofp;

    PropertyList &props = Ref_cast<PropertyList>(list);

    UInt16 property;
    io.mapRequired("property", property);

    if (property == OFPPMPT_EXPERIMENTER) {
      UInt32 experimenter;
      io.mapRequired("experimenter", experimenter);

      UInt32 expType;
      io.mapRequired("exp_type", expType);

      ByteList expData;
      io.mapRequired("data", expData);

      props.add(PortModPropertyExperimenter{experimenter, expType, expData});

    } else {
      ByteList propData;
      io.mapRequired("data", propData);
      props.add(UnrecognizedProperty{property, propData});
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortModPropertyRange> {
  using iterator = ofp::detail::PortModPropertyIterator;

  static iterator begin(IO &io, ofp::detail::PortModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    auto it = ofp::detail::PortModPropertyIterator{props.begin()};
    skip(it, end(io, range));
    return it;
  }

  static iterator end(IO &io, ofp::detail::PortModPropertyRange &range) {
    ofp::PropertyRange props = Ref_cast<ofp::PropertyRange>(range);
    return ofp::detail::PortModPropertyIterator{props.end()};
  }

  static void next(iterator &iter, iterator iterEnd) {
    ++iter;
    skip(iter, iterEnd);
  }

  static void skip(iterator &iter, iterator iterEnd) {
    for (; iter < iterEnd; ++iter) {
      ofp::UInt16 type = iter->type();
      if (type >= ofp::OFPPMPT_UNUSED_MIN)
        break;
    }
  }
};

template <>
struct SequenceTraits<ofp::detail::PortModPropertyList> {
  static size_t size(IO &io, ofp::detail::PortModPropertyList &props) {
    return 0;
  }

  static ofp::detail::PortModPropertyInserter &element(
      IO &io, ofp::detail::PortModPropertyList &props, size_t index) {
    return Ref_cast<ofp::detail::PortModPropertyInserter>(props);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTMOD_H_
