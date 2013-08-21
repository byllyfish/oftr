#ifndef OFP_OXMDISPATCH_H
#define OFP_OXMDISPATCH_H

namespace ofp { // <namespace ofp>

struct OXMDelegate {

    template <class Value>
    void on(OXMType type)
    {
        Value x = get<Value>();
        list.add(x);
    }
};



// All OXMTypes should have an internal ID value in 0..N-1. If necessary, we can
// hash, tree set the OXMType to ID lookup. ID immediately gives you info, etc.

type.dispatch(delegate);

template <class Delegate>
void OXMDispatch(OXMType type)
{

    switch (type) {
    case ofp::OFB_IN_PORT::type() :
    	Delegate::on<ofp::OFB_IN_PORT>();
        break;
    }

    case ofp::OFB_ETH_TYPE::type() : {
        llvm::yaml::Hex16 val = item.value<ofp::OFB_ETH_TYPE>().value();
        io.mapRequired("value", val);
        break;
    }

    case ofp::OFB_IPV4_DST::type() : {
        auto val = item.value<ofp::OFB_IPV4_DST>().value();
        io.mapRequired("value", val);
        break;
    }
    }
}

} // </namespace ofp>

#endif // OFP_OXMDISPATCH_H
