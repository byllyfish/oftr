#ifndef OFP_CONTROLLER_H
#define OFP_CONTROLLER_H

namespace ofp { // <namespace ofp>
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
} // </namespace ofp>

#endif // OFP_CONTROLLER_H
