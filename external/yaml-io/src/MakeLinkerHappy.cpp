
namespace llvm { // <namespace llvm>

class raw_ostream;

void report_fatal_error(char const*, bool);
raw_ostream &dbgs();
bool isCurrentDebugType(char const*);
void llvm_unreachable_internal(char const*, char const*, unsigned int);

void report_fatal_error(char const*, bool)
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

bool DebugFlag;

} // </namespace llvm>