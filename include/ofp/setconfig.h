//  ===== ---- ofp/setconfig.h -----------------------------*- C++ -*- =====  //
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
/// \brief Defines the SetConfig and SetConfigBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SETCONFIG_H
#define OFP_SETCONFIG_H

#include "ofp/header.h"
#include "ofp/message.h"

namespace ofp { // <namespace ofp>

class SetConfigBuilder;

class SetConfig {
public:
	static constexpr OFPType type() { return OFPT_SET_CONFIG; }
	static const SetConfig *cast(const Message *message);

	SetConfig() : header_{type()} {}

	UInt16 flags() const { return flags_; }
	UInt16 missSendLen() const { return missSendLen_; }

private:
	Header header_;
	Big16 flags_ = 0;
	Big16 missSendLen_ = 128;

	bool validateLength(size_t length) const;

	friend class SetConfigBuilder;
	friend struct llvm::yaml::MappingTraits<SetConfig>;
	friend struct llvm::yaml::MappingTraits<SetConfigBuilder>;
};

static_assert(sizeof(SetConfig) == 12, "Unexpected size.");
static_assert(IsStandardLayout<SetConfig>(), "Expected standard layout.");

class SetConfigBuilder {
public:
	SetConfigBuilder() = default;

	void setFlags(UInt16 flags);
	void setMissSendLen(UInt16 missSendLen);
	
	UInt32 send(Writable *channel);

private:
	SetConfig msg_;

	friend struct llvm::yaml::MappingTraits<SetConfigBuilder>;
};

} // </namespace ofp>

#endif // OFP_SETCONFIG_H
