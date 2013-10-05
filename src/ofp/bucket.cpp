#include "ofp/bucket.h"

using namespace ofp;


Bucket::Bucket(const ActionRange &actions)
	: len_{SizeWithoutActionRange + actions.size()}, actions_{actions}
{
}

