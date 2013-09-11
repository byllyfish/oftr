#include "ofp/version.h"

#define MAKE_VERSION(a, b)		#a "." #b

const char *const VERSION_STRING = MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR);