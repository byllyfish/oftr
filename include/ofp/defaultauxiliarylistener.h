//  ===== ---- ofp/defaultauxiliarylistener.h --------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the DefaultAuxiliaryListener class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_DEFAULTAUXILIARYLISTENER_H
#define OFP_DEFAULTAUXILIARYLISTENER_H

#include "ofp/channellistener.h"

namespace ofp { // <namespace ofp>

class DefaultAuxiliaryListener : public ChannelListener {
public:

	virtual void onChannelUp(Channel *channel) override 
	{
		log::debug("DefaultAuxiliaryListener onChannelUp");
	}

	virtual void onMessage(const Message *message) override 
	{
		log::debug("DefaultAuxiliaryListener onMessage");
	}

private:
	
};

} // </namespace ofp>

#endif // OFP_DEFAULTAUXILIARYLISTENER_H
