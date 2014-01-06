#include "ofp/yaml/outputjson.h"

using namespace ofp::yaml;
using namespace llvm;

OutputJson::OutputJson(llvm::raw_ostream &yout, void *ctxt)
    : IO{ctxt}, Out(yout), Column{0}, NeedComma{false} {}

OutputJson::~OutputJson() {}

bool OutputJson::outputting() { return true; }

void OutputJson::beginMapping() {
  output("{");
  NeedComma = false;
}

bool OutputJson::mapTag(StringRef Tag, bool Use) { return Use; }

void OutputJson::endMapping() { output("}"); }

bool OutputJson::preflightKey(const char *Key, bool Required,
                              bool SameAsDefault, bool &UseDefault, void *&) {
  UseDefault = false;
  if (Required || !SameAsDefault) {
    if (NeedComma)
      output(",");
    this->paddedKey(Key);
    return true;
  }
  return false;
}

void OutputJson::postflightKey(void *) { NeedComma = true; }

unsigned OutputJson::beginSequence() {
  output("[");
  NeedComma = false;
  return 0;
}

void OutputJson::endSequence() { output("]"); }

bool OutputJson::preflightElement(unsigned, void *&) { return true; }

void OutputJson::postflightElement(void *) { NeedComma = true; }

unsigned OutputJson::beginFlowSequence() {
  output("[");
  NeedComma = false;
  return 0;
}

void OutputJson::endFlowSequence() { output("]"); }

bool OutputJson::preflightFlowElement(unsigned, void *&) {
  if (NeedComma)
    output(",");
  if (Column > 120) {
    outputNewLine();
  }
  return true;
}

void OutputJson::postflightFlowElement(void *) { NeedComma = true; }

void OutputJson::beginEnumScalar() {}

bool OutputJson::matchEnumScalar(const char *Str, bool Match) {
  if (Match) { // TODO(bfish): Check out caller...
    output("\"");
    output(Str);
    output("\"");
  }
  return false;
}

void OutputJson::endEnumScalar() {}

bool OutputJson::beginBitSetScalar(bool &DoClear) {
  output("[");
  return true;
}

bool OutputJson::bitSetMatch(const char *Str, bool Matches) {
  if (Matches) {
    if (NeedComma)
      output(",");
    output(Str);
    NeedComma = true;
  }
  return false;
}

void OutputJson::endBitSetScalar() { output("]"); }

void OutputJson::scalarString(StringRef &S) {
  // If string begins with a leading zero, it may be in hexadecimal format.
  unsigned long long u;
  if (!llvm::getAsUnsignedInteger(S, 0, u)) {
    output(std::to_string(u));
  } else {
    // FIXME(bfish): Handle special chars in scalar strings!
    output("\"");
    output(S);
    output("\"");
  }
}

void OutputJson::setError(const Twine &message) {}

bool OutputJson::canElideEmptySequence() { return false; }

void OutputJson::output(StringRef s) {
  Column += s.size();
  Out << s;
}

void OutputJson::outputUpToEndOfLine(StringRef s) { output(s); }

void OutputJson::outputNewLine() {
  Out << "\n";
  Column = 0;
}

void OutputJson::paddedKey(StringRef key) {
  output("\"");
  output(key);
  output("\":");
}

void OutputJson::beginDocuments() { outputUpToEndOfLine("---\n"); }

bool OutputJson::preflightDocument(unsigned index) {
  if (index > 0)
    outputUpToEndOfLine("\n---\n");
  return true;
}

void OutputJson::postflightDocument() {}

void OutputJson::endDocuments() { output("\n...\n"); }
