#ifndef OFP_MATCH_H
#define OFP_MATCH_H

namespace ofp { // <namespace ofp>

class Match {
public:
    explicit Match(OXMRange range) : oxm_{range} {}
    explicit Match(const deprecated::StandardMatch *match) : oxm_{match.toOXMList()} {}



private:
    OXMList oxm_;
};

} // </namespace ofp>

#endif // OFP_MATCH_H
