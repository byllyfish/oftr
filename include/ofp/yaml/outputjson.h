//  ===== ---- ofp/yaml/outputjson.h -----------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//  
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the yaml::OutputJson class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_YAML_OUTPUTJSON_H_
#define OFP_YAML_OUTPUTJSON_H_

#include "ofp/yaml/yllvm.h"

namespace ofp {
namespace yaml {

OFP_BEGIN_IGNORE_PADDING

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
  void paddedKey(llvm::StringRef key);

  llvm::raw_ostream       &Out;
  bool                     NeedComma;
};

OFP_END_IGNORE_PADDING

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

}  // namespace yaml
}  // namespace ofp

#endif  // OFP_YAML_OUTPUTJSON_H_
