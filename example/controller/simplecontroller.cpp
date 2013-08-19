#include "simplecontroller.h"

namespace controller { // <namespace controller>


void SimpleController::onPacketIn(Channel *source, const PacketIn *message)
{
    log::debug("SimpleController - PacketIn from:", source);

}


} // </namespace controller>