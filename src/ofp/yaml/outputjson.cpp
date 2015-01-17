// Copyright 2014-present Bill Fisher. All rights reserved.

#include "ofp/yaml/outputjson.h"

using namespace ofp::yaml;
using namespace llvm;

OutputJson::OutputJson(llvm::raw_ostream &yout, void *ctxt)
    : IO{ctxt}, Out(yout), NeedComma{false} {}

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
    if (NeedComma) output(",");
    this->paddedKey(Key);
    return true;
  }
  return false;
}

void OutputJson::postflightKey(void *SaveInfo) { NeedComma = true; }

unsigned OutputJson::beginSequence() {
  output("[");
  NeedComma = false;
  return 0;
}

void OutputJson::endSequence() { output("]"); }

bool OutputJson::preflightElement(unsigned, void *&) {
  if (NeedComma) output(",");
  return true;
}

void OutputJson::postflightElement(void *SaveInfo) { NeedComma = true; }

unsigned OutputJson::beginFlowSequence() {
  output("[");
  NeedComma = false;
  return 0;
}

void OutputJson::endFlowSequence() { output("]"); }

bool OutputJson::preflightFlowElement(unsigned, void *&) {
  if (NeedComma) output(",");
  return true;
}

void OutputJson::postflightFlowElement(void *SaveInfo) { NeedComma = true; }

void OutputJson::beginEnumScalar() {}

bool OutputJson::matchEnumScalar(const char *Str, bool Match) {
  if (Match) {  // TODO(bfish): Check out caller...
    output("\"");
    output(Str);
    output("\"");
  }
  return false;
}

void OutputJson::endEnumScalar() {}

bool OutputJson::beginBitSetScalar(bool &DoClear) {
  output("[");
  NeedComma = false;
  return true;
}

bool OutputJson::bitSetMatch(const char *Str, bool Matches) {
  if (Matches) {
    if (NeedComma) output(",");
    output("\"");
    output(Str);
    output("\"");
    NeedComma = true;
  }
  return false;
}

void OutputJson::endBitSetScalar() { output("]"); }

bool OutputJson::bitSetMatchOther(uint32_t &Val) {
  if (Val != 0) {
    if (NeedComma)
      output(",");
    char buf[16];
    auto len = format("\"0x%08X\"", Val).print(buf, sizeof(buf));
    this->output(StringRef{buf, len});
    NeedComma = true;
  }
  return false;
}

// Return offset of next unsafe character -- either a control character, \ or ".
static StringRef::size_type findUnsafe(const StringRef &s,
                                       StringRef::size_type pos) {
  const char *data = s.data();
  StringRef::size_type len = s.size();
  for (StringRef::size_type i = pos; i < len; ++i) {
    unsigned ch = static_cast<unsigned>(data[i]);
    if (ch < 0x20 || ch == '\\' || ch == '"') return i;
  }
  return StringRef::npos;
}

const char *const ControlReplacement[] = {
    "u0000", "u0001", "u0002", "u0003", "u0004", "u0005", "u0006", "u0007",
    "b",     "t",     "n",     "u000B", "f",     "r",     "u000E", "u000F",
    "u0010", "u0011", "u0012", "u0013", "u0014", "u0015", "u0016", "u0017",
    "u0018", "u0019", "u001A", "u001B", "u001C", "u001D", "u001E", "u001F",
};

void OutputJson::scalarString(StringRef &S, bool MustQuote) {
  // Output value wrapped in double-quotes. Escape any embedded double-quotes
  // or backslashes in the string. Replace control characters (ASCII < 32)
  // with \b, \t, \n, \f, \r or \uxxxx.
  output("\"");
  StringRef::size_type pos = 0;
  StringRef::size_type offset = findUnsafe(S, pos);
  if (offset == StringRef::npos) {
    // No need to escape characters in the string.
    output(S);
  } else {
    // At least one character found that must be escaped. Deal with the first
    // one, then loop through the string looking for the remaining unsafe
    // characters.
    output(S.substr(pos, offset));
    output("\\");
    unsigned ch = static_cast<unsigned>(S[offset]);
    if (ch < 0x20) {  // control character?
      output(ControlReplacement[ch]);
      pos = offset + 1;
    } else {
      pos = offset;
    }

    while ((offset = findUnsafe(S, offset + 1)) != StringRef::npos) {
      output(S.substr(pos, offset - pos));
      output("\\");
      ch = static_cast<unsigned>(S[offset]);
      if (ch <= 0x20) {  // control character?
        output(ControlReplacement[ch]);
        pos = offset + 1;
      } else {
        pos = offset;
      }
    }

    output(S.substr(pos));
  }
  output("\"");  // closing quote
}

void OutputJson::setError(const Twine &message) {}

bool OutputJson::canElideEmptySequence() { return false; }

void OutputJson::output(StringRef s) { Out << s; }

void OutputJson::paddedKey(StringRef key) {
  output("\"");
  output(key);
  output("\":");
}

void OutputJson::beginDocuments() { /* output("---\n"); */
}

bool OutputJson::preflightDocument(unsigned index) {
  /* if (index > 0)
    output("\n---\n"); */
  return true;
}

void OutputJson::postflightDocument() { /*output("\n");*/
}

void OutputJson::endDocuments() { /* output("\n...\n"); */
}
