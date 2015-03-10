// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef TOOLS_OFPX_OFPX_HELP_H_
#define TOOLS_OFPX_OFPX_HELP_H_

#include "./ofpx.h"

namespace ofpx {

// ofpx help [<text>]
//
// Access built-in help features.

class Help : public Subprogram {
 public:
  enum class ExitStatus { Success = 0 };

  int run(int argc, const char *const *argv) override;

 private:
  // --- Command-line Arguments ---

  void listFields();
};

}  // namespace ofpx

#endif  // TOOLS_OFPX_OFPX_HELP_H_
