#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ErrorHandling.h"
#include <system_error>

namespace llvm { // <namespace llvm>

class raw_ostream;

void report_fatal_error(char const*, bool)
{
    std::abort();
}

void report_fatal_error(const std::string &, bool)
{
    std::abort();
}

void report_bad_alloc_error(const char *Reason, bool GenCrashDiag) {
    std::abort();
}


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
{
    std::abort();
}


std::error_code mapWindowsError(unsigned EV) {
	return std::error_code(EV, std::system_category());
}

bool DebugFlag;


namespace sys {

std::string getDefaultTargetTriple() { return ""; }
StringRef getHostCPUName() { return ""; }

}  // namespace sys
} // </namespace llvm>
