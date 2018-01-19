// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_OUTPUTJSON_H_
#define OFP_YAML_OUTPUTJSON_H_

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace yaml {

OFP_BEGIN_IGNORE_PADDING

class OutputJson : public llvm::yaml::IO {
 public:
  explicit OutputJson(llvm::raw_ostream &yout, void *ctxt = NULL);
  virtual ~OutputJson();

  bool outputting() override;
  bool outputtingJson() override { return true; }

  unsigned beginSequence() override;
  bool preflightElement(unsigned Index, void *&SaveInfo) override;
  void postflightElement(void *SaveInfo) override;
  void endSequence() override;
  bool canElideEmptySequence() override;

  unsigned beginFlowSequence() override;
  bool preflightFlowElement(unsigned Index, void *&SaveInfo) override;
  void postflightFlowElement(void *SaveInfo) override;
  void endFlowSequence() override;

  bool mapTag(llvm::StringRef Tag, bool Default = false) override;
  void beginMapping() override;
  void endMapping() override;
  bool preflightKey(const char *Key, bool Required, bool, bool &UseDefault,
                    void *&SaveInfo) override;
  void postflightKey(void *SaveInfo) override;
  std::vector<llvm::StringRef> keys() override { return {}; }

  void beginFlowMapping() override { beginMapping(); }
  void endFlowMapping() override { endMapping(); }

  void beginEnumScalar() override;
  bool matchEnumScalar(const char *Str, bool) override;
  bool matchEnumFallback() override;
  void endEnumScalar() override;

  bool beginBitSetScalar(bool &DoClear) override;
  bool bitSetMatch(const char *Str, bool) override;
  void endBitSetScalar() override;
  bool bitSetMatchOther(uint32_t &) override;
  llvm::StringRef bitSetCaseUnmatched() override;

  void scalarString(llvm::StringRef &S, bool) override;
  void scalarJson(llvm::StringRef s) override { output(s); }
  void blockScalarString(llvm::StringRef &S) override { scalarString(S, true); }

  void setError(const llvm::Twine &message) override;

 public:
  // These are only used by operator<<. They could be private
  // if that templated operator could be made a friend.
  void beginDocuments();
  bool preflightDocument(unsigned);
  void postflightDocument();
  void endDocuments();

 private:
  void output(llvm::StringRef s);
  void paddedKey(llvm::StringRef key);

  llvm::raw_ostream &Out;
  bool NeedComma;
};

OFP_END_IGNORE_PADDING

// Define non-member operator<< so that OutputJson can stream out a map.
// (Adapted from llvm::yaml.)
template <typename T>
inline typename std::enable_if<
    llvm::yaml::has_MappingTraits<T, llvm::yaml::EmptyContext>::value,
    OutputJson &>::type
operator<<(OutputJson &yout, T &map) {
  llvm::yaml::EmptyContext Ctx;
  yout.beginDocuments();
  if (yout.preflightDocument(0)) {
    yamlize(yout, map, true, Ctx);
    yout.postflightDocument();
  }
  yout.endDocuments();
  return yout;
}

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_OUTPUTJSON_H_
