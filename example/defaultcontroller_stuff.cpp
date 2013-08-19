/**
 *  Listen for incoming OpenFlow connections on the default port. After an
 *  OpenFlow switch connects and finishes the protocol handshake, a controller
 *  instance will receive a `start` event which it can use to register for
 *  events or send commands to the switch.
 */
template <class Controller>
Exception
runDefaultController(ProtocolVersions versions = ProtocolVersions::All)
{
    return runController([]() {
        return new controller::details::Listener<Controller>;
    }, versions);
}




namespace controller { // <namespace controller>

class Provider {
public:
	virtual Channel *channel() const;

	template <class Mesg>
	virtual void listen(memberfunction);

	template <class MesgBuilder>
	virtual void send(const MesgBuilder &mesg, memberfunction, memberfunction);

	template <class MesgBuilder>
	virtual void sendMulti(const MesgBuilder &mesg, memberfunctionwithlast, memberfunction);
};



} // </namespace controller>




class SimpleController {
public:
	
	void start(ofp::controller::Provider<SimpleController> *provider);
	void stop();

#if 0
	void packetIn(ofp::Channel *source, ofp::PacketIn *message);
	void flowRemoved(ofp::Channnel *source, ofp::FlowRemoved *message);
	void portStatus(ofp::Channel *source, ofp::PortStatus *message);
	void reply(ofp::Channel *source, const ofp::Message *message);

private:
	ReplyTracker<SimpleController> track_;

	void trackReply(message, handler, errorHandler);
#endif
};



int main(int argc, char **argv)
{
    auto ex = ofp::runDefaultController<SimpleController>();

    if (ex) {
        std::cerr << "Error running controller: " << ex << std::eol;
        return 1;
    }

    return 0;
}
