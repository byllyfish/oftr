//  ===== ---- ofp/actions.h -------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines classes for OpenFlow Actions.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONS_H
#define OFP_ACTIONS_H

#include "ofp/actiontype.h"
#include "ofp/padding.h"
#include "ofp/oxmtype.h"

namespace ofp { // <namespace ofp>


namespace detail { // <namespace detail>

template <UInt32 ActType>
class EmptyAction {
public:
	constexpr static ActionType type() { return ActionType(ActType, 8); }

	constexpr EmptyAction() {}

private:
	const ActionType type_ = type();
	const Padding<4> pad_;
};

} // </namespace detail>


/*-------------------------------------------------------------------------*//**
 \brief Concrete types for various empty actions.
 *-------------------------------------------------------------------------*///*

using AT_COPY_TTL_OUT = detail::EmptyAction<ActionType::AT_COPY_TTL_OUT>;
using AT_COPY_TTL_IN = detail::EmptyAction<ActionType::AT_COPY_TTL_IN>;
using AT_DEC_MPLS_TTL = detail::EmptyAction<ActionType::AT_DEC_MPLS_TTL>;
using AT_POP_VLAN = detail::EmptyAction<ActionType::AT_POP_VLAN>;
using AT_POP_MPLS = detail::EmptyAction<ActionType::AT_POP_MPLS>;
using AT_DEC_NW_TTL = detail::EmptyAction<ActionType::AT_DEC_NW_TTL>;
using AT_POP_PBB = detail::EmptyAction<ActionType::AT_POP_PBB>;

static_assert(sizeof(AT_COPY_TTL_OUT) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_COPY_TTL_OUT>(), "Unexpected layout.");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_OUTPUT action.
 *-------------------------------------------------------------------------*///*

class AT_OUTPUT {
public:

	constexpr static ActionType type() { return ActionType(ActionType::AT_OUTPUT, 8); }

	constexpr AT_OUTPUT(UInt16 port, UInt16 maxlen)
		: port_{port}, maxlen_{maxlen} {}
		
	UInt16 port() const { return port_; }
	UInt16 maxlen() const { return maxlen_; }

private:
	const ActionType type_ = type();
	const Big16 port_;
	const Big16	maxlen_;
};

static_assert(sizeof(AT_OUTPUT) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_OUTPUT>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_SET_MPLS_TTL action.
 *-------------------------------------------------------------------------*///*

class AT_SET_MPLS_TTL {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_SET_MPLS_TTL, 8); }

	constexpr explicit AT_SET_MPLS_TTL(UInt16 ttl)
		: ttl_{ttl} {}
		
	UInt16 ttl() const { return ttl_; }

private:
	const ActionType type_ = type();
	const Big16 ttl_;
	const Padding<2> pad_;
};

static_assert(sizeof(AT_SET_MPLS_TTL) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_MPLS_TTL>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_PUSH_VLAN action.
 *-------------------------------------------------------------------------*///*

class AT_PUSH_VLAN {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_PUSH_VLAN, 8); }

	constexpr explicit AT_PUSH_VLAN(UInt16 vlan)
		: vlan_{vlan} {}
		
	UInt16 vlan() const { return vlan_; }

private:
	const ActionType type_ = type();
	const Big16 vlan_;	
	const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_VLAN) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_VLAN>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_PUSH_MPLS action.
 *-------------------------------------------------------------------------*///*

class AT_PUSH_MPLS {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_PUSH_MPLS, 8); }

	constexpr explicit AT_PUSH_MPLS(UInt32 mpls)
		: mpls_{mpls} {}
		
	UInt32 mpls() const { return mpls_; }

private:
	const ActionType type_ = type();
	const Big32 mpls_;
};

static_assert(sizeof(AT_PUSH_MPLS) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_MPLS>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_SET_QUEUE action.
 *-------------------------------------------------------------------------*///*

class AT_SET_QUEUE {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_SET_QUEUE, 8); }

	constexpr explicit AT_SET_QUEUE(UInt8 queue)
		: queue_{queue} {}
		
	UInt8 queue() const { return queue_; }

private:
	const ActionType type_ = type();
	const Big8 queue_;
	const Padding<3> pad_;
};

static_assert(sizeof(AT_SET_QUEUE) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_QUEUE>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_GROUP action.
 *-------------------------------------------------------------------------*///*

class AT_GROUP {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_GROUP, 8); }

	constexpr explicit AT_GROUP(UInt8 group)
		: group_{group} {}
		
	UInt8 group() const { return group_; }

private:
	const ActionType type_ = type();
	const Big8 group_;
	const Padding<3> pad_;
};

static_assert(sizeof(AT_GROUP) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_GROUP>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_SET_NW_TTL action.
 *-------------------------------------------------------------------------*///*

class AT_SET_NW_TTL {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_SET_NW_TTL, 8); }

	constexpr explicit AT_SET_NW_TTL(UInt8 ttl)
		: ttl_{ttl} {}
		
	UInt8 ttl() const { return ttl_; }

private:
	const ActionType type_ = type();
	const Big8 ttl_;
	const Padding<3> pad_;
};

static_assert(sizeof(AT_SET_NW_TTL) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_SET_NW_TTL>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_SET_NW_TTL action.
 *-------------------------------------------------------------------------*///*

class AT_PUSH_PBB {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_PUSH_PBB, 8); }

	constexpr explicit AT_PUSH_PBB(UInt16 ethertype)
		: ethertype_{ethertype} {}
		
	UInt16 ethertype() const { return ethertype_; }

private:
	const ActionType type_ = type();
	const Big16 ethertype_;
	const Padding<2> pad_;
};

static_assert(sizeof(AT_PUSH_PBB) == 8, "Unexpected size.");
static_assert(IsStandardLayout<AT_PUSH_PBB>(), "Unexpected layout");

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_EXPERIMENTER action.
 *-------------------------------------------------------------------------*///*

class AT_EXPERIMENTER {
public:
	constexpr static ActionType type() { return ActionType(ActionType::AT_EXPERIMENTER, 8); }

	constexpr explicit AT_EXPERIMENTER(UInt32 experimenterid)
		: experimenterid_{experimenterid} {}
		
	UInt32 experimenterid() const { return experimenterid_; }

private:
	const ActionType type_ = type();
	const Big32 experimenterid_;
};

/*-------------------------------------------------------------------------*//**
 \brief Concrete type for AT_SET_FIELD action.
 *-------------------------------------------------------------------------*///*

template <class ValueType>
class AT_SET_FIELD {
public:

	constexpr static ActionType type() { return ActionType(ActionType::AT_SET_FIELD, 8 + sizeof(Padded<ValueType>)); }
	
	constexpr AT_SET_FIELD(const ValueType &value) 
		: value_{value} {}

	ValueType value() const { return value_; }

private:
	const ActionType type_ = type();
	const OXMType omxtype_ = ValueType::type();
	const Padded<ValueType> value_;
};


} // </namespace ofp>


#endif //OFP_ACTIONS_H
