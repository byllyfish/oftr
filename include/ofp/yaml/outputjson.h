#ifndef OFP_YAML_OUTPUTJSON_H
#define OFP_YAML_OUTPUTJSON_H

#include "ofp/yaml/yllvm.h"

namespace ofp { // <namespace ofp>
namespace yaml { // <namespace yaml>

class OutputJson : public llvm::yaml::IO {
public:
  OutputJson(llvm::raw_ostream &yout, void *ctxt=NULL);
  virtual ~OutputJson();

  virtual bool outputting() override;

  virtual unsigned beginSequence() override;
  virtual bool preflightElement(unsigned, void *&) override;
  virtual void postflightElement(void*) override;
  virtual void endSequence() override;
  virtual bool canElideEmptySequence() override;

  virtual unsigned beginFlowSequence() override;
  virtual bool preflightFlowElement(unsigned, void *&) override;
  virtual void postflightFlowElement(void*) override;
  virtual void endFlowSequence() override;

  virtual bool mapTag(llvm::StringRef Tag, bool Default=false) override;
  virtual void beginMapping() override;
  virtual void endMapping() override;
  virtual bool preflightKey(const char*, bool, bool, bool &, void *&) override;
  virtual void postflightKey(void*) override;

  virtual void beginEnumScalar() override;
  virtual bool matchEnumScalar(const char*, bool) override;
  virtual void endEnumScalar() override;

  virtual bool beginBitSetScalar(bool &) override;
  virtual bool bitSetMatch(const char*, bool) override;
  virtual void endBitSetScalar() override;

  virtual void scalarString(llvm::StringRef &) override;

  virtual void setError(const llvm::Twine &) override;

public:
  // These are only used by operator<<. They could be private
  // if that templated operator could be made a friend.
  void beginDocuments();
  bool preflightDocument(unsigned);
  void postflightDocument();
  void endDocuments();

private:
  void output(llvm::StringRef s);
  void outputUpToEndOfLine(llvm::StringRef s);
  void newLineCheck();
  void outputNewLine();
  void paddedKey(llvm::StringRef key);

  llvm::raw_ostream       &Out;
  int                      Column;
  bool                     NeedComma;
};


// Define non-member operator<< so that OutputJson can stream out a map.
// (Adapted from llvm::yaml.)
template <typename T>
inline
typename llvm::enable_if_c<llvm::yaml::has_MappingTraits<T>::value,OutputJson &>::type
operator<<(OutputJson &yout, T &map) {
  yout.beginDocuments();
  if ( yout.preflightDocument(0) ) {
    yamlize(yout, map, true);
    yout.postflightDocument();
  }
  yout.endDocuments();
  return yout;
}

} // </namespace yaml>
} // </namespace ofp>

#endif // OFP_YAML_OUTPUTJSON_H
