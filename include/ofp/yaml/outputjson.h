// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_OUTPUTJSON_H_
#define OFP_YAML_OUTPUTJSON_H_

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace yaml {

OFP_BEGIN_IGNORE_PADDING

class OutputJson : public llvm::yaml::IO {
 public:
  OutputJson(llvm::raw_ostream &yout, void *ctxt = NULL);
  virtual ~OutputJson();

  virtual bool outputting() override;
  virtual bool outputtingJson() override { return true; }

  virtual unsigned beginSequence() override;
  virtual bool preflightElement(unsigned Index, void *&SaveInfo) override;
  virtual void postflightElement(void *SaveInfo) override;
  virtual void endSequence() override;
  virtual bool canElideEmptySequence() override;

  virtual unsigned beginFlowSequence() override;
  virtual bool preflightFlowElement(unsigned Index, void *&SaveInfo) override;
  virtual void postflightFlowElement(void *SaveInfo) override;
  virtual void endFlowSequence() override;

  virtual bool mapTag(llvm::StringRef Tag, bool Default = false) override;
  virtual void beginMapping() override;
  virtual void endMapping() override;
  virtual bool preflightKey(const char *Key, bool Required, bool,
                            bool &UseDefault, void *&SaveInfo) override;
  virtual void postflightKey(void *SaveInfo) override;

  virtual void beginEnumScalar() override;
  virtual bool matchEnumScalar(const char *Str, bool) override;
  virtual void endEnumScalar() override;

  virtual bool beginBitSetScalar(bool &DoClear) override;
  virtual bool bitSetMatch(const char *Str, bool) override;
  virtual void endBitSetScalar() override;

  virtual void scalarString(llvm::StringRef &S, bool) override;
  virtual void scalarJson(llvm::StringRef s) override { output(s); }

  virtual void setError(const llvm::Twine &message) override;

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
inline typename std::enable_if<llvm::yaml::has_MappingTraits<T>::value,
                               OutputJson &>::type
operator<<(OutputJson &yout, T &map) {
  yout.beginDocuments();
  if (yout.preflightDocument(0)) {
    yamlize(yout, map, true);
    yout.postflightDocument();
  }
  yout.endDocuments();
  return yout;
}

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_OUTPUTJSON_H_
