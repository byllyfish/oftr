#include "llvm/ADT/StringRef.h"

namespace llvm { // <namespace llvm>

class raw_ostream;

void report_fatal_error(char const*, bool);
void report_fatal_error(const std::string &, bool);
raw_ostream &dbgs();
bool isCurrentDebugType(char const*);
void llvm_unreachable_internal(char const*, char const*, unsigned int);
std::error_code mapWindowsError(unsigned EV);

void report_fatal_error(char const*, bool)
{}

void report_fatal_error(const std::string &, bool)
{}


raw_ostream &dbgs()
{ 
	// Should not be called.
	raw_ostream *os = nullptr;
	return *os;
}


bool isCurrentDebugType(char const*)
{
	return false;
}

void llvm_unreachable_internal(char const*, char const*, unsigned int)
{}


std::error_code mapWindowsError(unsigned EV) {
	return std::error_code(EV, std::system_category());
}

bool DebugFlag;


namespace sys {

std::string getDefaultTargetTriple() { return ""; }
StringRef getHostCPUName() { return ""; }

}  // namespace sys
} // </namespace llvm>
