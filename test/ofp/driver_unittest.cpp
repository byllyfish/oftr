//  ===== ---- ofp/driver_unittest.cpp ---------------------*- C++ -*- =====  //
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
/// \brief Implements unit tests for Driver class.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/unittest.h"
#include "ofp/driver.h"
#include "ofp/exception.h"

using namespace ofp;

class MockChannelListener : public ChannelListener {
public:
    void onChannelUp(Channel *channel) override
    {
    }
    void onChannelDown(Channel *channel) override
    {
    }
    void onMessage(const Message *message) override;

    void onException(const Exception *error) override
    {
    }
    void onTimer(UInt32 timerID) override {
    }
};


void MockChannelListener::onMessage(const Message *message)
{
}


TEST(driver, test)
{
    log::set(&std::cerr);
    
    Driver driver;

    driver.listen(Driver::Controller, nullptr, IPv6Endpoint{OFP_DEFAULT_PORT}, 
                  ProtocolVersions{}, []{
        return new MockChannelListener;
    });

    //driver.run();
}
