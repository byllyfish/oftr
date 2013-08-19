
namespace llvm {

class raw_ostream;

void report_fatal_error(char const*, bool);

raw_ostream &dbgs();


void report_fatal_error(char const*, bool)
{}


raw_ostream &dbgs()
{ 
	// Should not be called.
	raw_ostream *os = nullptr;
	return *os;
}

}