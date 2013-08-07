

#ifndef OFP_MATCHFIELD_H
#define OFP_MATCHFIELD_H

#include "ofp/endian.h"

namespace ofp { // <namespace ofp>

// Call this oxm_value cclasses?
// oxm_type = (Class | Field | HM | Length) is constant.

template <
	UInt16 Class,
	UInt8 Field,
	class Type,
	UInt8 Bits,
	bool Mask, 
	const char *Description
>
class MatchField {
public:
	using InternalType = Type;
	
	constexpr static const UInt32 oxm_type = (Class << 16UL) | Field;
	constexpr static const UInt8 oxm_bits = Bits;
	constexpr static const bool oxm_mask = Mask;
	constexpr static const char *oxm_description = Description;

	static_assert(sizeof(Syntax) * 8 <= Bits);
	
	MatchField(Type value) : value_{value} {}
	operator Type() const { return value_; }
	void operator=(Type value) { value_ = value; }
	
protected:
	InternalType value_;
};

} // </namespace ofp>

#endif //OFP_MATCHFIELD_H



using ofp_ofb_in_port_prereq = ...
// Define these in openflow_match_prereqs.h

using ofp_ofb_in_port = MatchField<
	0x8000, 0, UInt32, 32, false,
	"Ingress port. Numerical representation of incoming port, starting at 1. "
	"This may be a physical or switch-defined logical port.">


// Build the MatchField's from a csv file. openflow_match_fields.csv
// experimenter_match_fields.h <= experimenter_match_fields.csv.
// experimenter_types.h
// experimenter_match_prereqs.h

// Have an option to set all descriptions to "" when compiling...


// Add a mandatory/required bool setting?
// Add a minVersion, maxVersion setting?

// How to check prereqs?

prereq<ofp_ofb_in_port>()

// Can this just be a function?
template <class Type, class FType, Pred pred>
class PrerequisiteUnary {
public:
	constexpr static const UInt32 id;
	using FieldType = FType;
	
	bool check(FType field);
};

//Prereq<> produces a functor.  bool operator(...)

Prereq<ofp_ofb_in_port> prereq;
field.prerequisite().check(...)

prereq(field1);
or 
prereq(field1, field2);
or
prereq(field1, field2, field3);
prereq()

// Idea:  In a FlowMatch structure, add a method to return the new FlowMatch
// with prereqs filled in. For some prereqs, like ETH TYPE=0x0800 or ETH TYPE=0x86dd
// we'll need to come up with an Internal MatchField type. ip_eth_type. It won't
// actually have a value; it's more of a predicate.
