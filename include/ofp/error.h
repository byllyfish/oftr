//  ===== ---- ofp/error.h ---------------------------------*- C++ -*- =====  //
//
//  This file is licensed under the Apache License, Version 2.0.
//  See LICENSE.txt for details.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the Error and ErrorBuilder classes.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ERROR_H
#define OFP_ERROR_H

#include "ofp/header.h"
#include "ofp/padding.h"
#include "ofp/bytelist.h"

namespace ofp { // <namespace ofp>

class Message;
class Writable;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E r r o r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements Error protocol message.

class Error {
public:
    static constexpr OFPType type()
    {
        return OFPT_ERROR;
    }

    static const Error *cast(const Message *message);

    Error() : header_{type()}
    {
    }

    UInt16 errorType() const { return type_; }
    UInt16 errorCode() const { return code_; }
    ByteRange errorData() const;

    bool validateLength(size_t length) const;

private:
    Header header_;
    Big16 type_;
    Big16 code_;

    friend class ErrorBuilder;
};

static_assert(sizeof(Error) == 12, "Unexpected size.");
static_assert(IsStandardLayout<Error>(), "Expected standard layout.");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
//   E r r o r B u i l d e r
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  //
/// \brief Implements Error protocol message builder.

class ErrorBuilder {
public:
    ErrorBuilder(UInt16 type = 0, UInt16 code = 0);

    void setErrorType(UInt16 type)
    {
        msg_.type_ = type;
    }

    void setErrorCode(UInt16 code)
    {
        msg_.code_ = code;
    }

    void setErrorData(const void *data, size_t length)
    {
        data_.set(data, length);
    }

    void setErrorData(const Message *message);

    void send(Writable *channel);

private:
    Error msg_;

    Padding<4> padNotPartOfPkt_;
    ByteList data_;
};

} // </namespace ofp>

#endif // OFP_ERROR_H
