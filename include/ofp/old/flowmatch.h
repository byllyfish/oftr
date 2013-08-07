#ifndef OFP_FLOWMATCH_H
#define OFP_FLOWMATCH_H

class FlowMatch {
public:
	
	
	Range<FlowMatchIterator> elements() const; 
};


class FlowMatchIterator {
public:
	
	const UInt8 *pos();
};

template <class T, std::pair P>
T Read<P>(FlowMatchIterator iter) const;

template <>
oxm_ipv4_src Read<oxm_ipv4_src::id>(FlowMatchIterator iter);


// To add a match field, define a class to hold the match field. Then
// define a specialization of the Read() function for a FlowMatchIterator.
// Also define a constant std::pair (or tuple) to represent the field.

// FieldID(x, y) 

template <class T>
void Write(FlowMatchBuilder builder, T field);


template <>
void Write(FlowMatchBuilder builder, oxm_ipv4_src field);


fieldtraits::id
fieldtraits::payload  // can infer length from this...
fieldtraits::mask
fieldtraits::prerequisites
fieldtraits::description


switch (iter.id()) {
case ofp_oxm_ipv4_src::id:
	auto f = iter.get<ofp_oxm_ipv4_src>();
	if (iter.hasMask())
		auto m = iter.getMask<ofp_oxm_ipv4_src>();
	dosomething(f);
	break;
}

constexpr UInt32 OXM_IPV4_SRC = oxm_ipv4_src::id;


oxm_ipv4_src foo(someaddr);
oxm_ipv4_src  mask(masky);

builder.add(foo);

or
builder.add(foo, mask);

// See tuple for std::get....

// Should use ofp::get, ofp::put ?

//OXM_IPv4_Source

#endif // OFP_FLOWMATCH_H

