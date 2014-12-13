#include "ofp/sys/dtls_utils.h"
#include "ofp/sys/asio_utils.h"

using namespace ofp;
using namespace ofp::sys;

const UInt8 kHandshakeType = 22;
const UInt8 kClientHelloRequest = 1;
const UInt8 kHelloVerifyRequest = 3;

static const char *dtlsTypeString(UInt8 type) {
  switch (type) {
    case 20:
      return "change_cipher_spec";
    case 21:
      return "alert";
    case 22:
      return "handshake";
    case 23:
      return "application_data";
    default:
      return "DTLS_unknown_type";
  }
}

static const char *dtlsVersionString(UInt32 version) {
  switch (version) {
    case 0xFEFF:
      return "DTLS1.0";
    case 0xFEFD:
      return "DTLS1.2";
    default:
      return "DTLS_unknown_version";
  }
}

// Source: ssl3.h
static const char *dtlsHandshakeType(UInt8 type) {
  switch (type) {
    case 0:
      return "hello_request";
    case 1:
      return "client_hello";
    case 2:
      return "server_hello";
    case 3:
      return "hello_verify_request";
    case 4:
      return "new_session_ticket";
    case 11:
      return "certificate";
    case 12:
      return "server_key_exchange";
    case 13:
      return "certificate_request";
    case 14:
      return "server_hello_done";
    case 15:
      return "certificate_verify";
    case 16:
      return "client_key_exchange";
    case 20:
      return "finished";
    case 22:
      return "certificate_status";
    case 23:
      return "supplemental_data";
    case 67:
      return "next_proto";
    case 203:
      return "encrypted_extensions";
    default:
      return "DTLS_unknown_handshake_type";
  }
}

size_t ofp::sys::DTLS_GetRecordLength(const void *buffer, size_t length) {
  const UInt8 *data = BytePtr(buffer);

  // Make sure we can read record header.
  if (length < DTLS1_RT_HEADER_LENGTH) return 0;

  // Check protocol version. DTLS 1.0 uses protocol version {254, 255}.
  // DTLS 1.2 uses protocol version { 254, 253 }.
  if ((data[1] != 0xFE) || (data[2] != 0xFF && data[2] != 0xFD)) return 0;

  // Extract length of the record.
  UInt32 recordLen = (UInt32_cast(data[11]) << 8) | data[12];

  // Record length should not exceed 2^14.
  if (recordLen > 16384) return 0;

  // Return total record length including header (if it fits in buffer).
  size_t result = recordLen + DTLS1_RT_HEADER_LENGTH;

  return (result > length) ? 0 : result;
}

std::string ofp::sys::DTLS_PrintRecord(const void *record, size_t length) {
  // Check that DTLS message is valid and read the record length.
  size_t recordLen = DTLS_GetRecordLength(record, length);
  if (recordLen == 0) {
    return std::string{"Invalid DTLS message: "} + RawDataToHex(record, length);
  }

  // Pull apart the DTLS record header.
  //
  //   uint8 type;
  //   uint16 version;
  //   uint16 epoch;
  //   uint48 sequence_number;
  //   uint16 length;

  const UInt8 *data = BytePtr(record);
  assert(length >= 13);

  UInt8 recordType = data[0];
  UInt32 recordVersion = (UInt32_cast(data[1]) << 8) | data[2];
  UInt32 recordEpoch = (UInt32_cast(data[3]) << 8) | data[4];
  UInt64 recordSeqNum =
      (UInt64_cast(data[5]) << 5 * 8) | (UInt64_cast(data[6]) << 4 * 8) |
      (UInt64_cast(data[7]) << 3 * 8) | (UInt64_cast(data[8]) << 2 * 8) |
      (UInt64_cast(data[9]) << 1 * 8) | UInt64_cast(data[10]);

  std::stringstream stream;

  stream << dtlsVersionString(recordVersion) << ' '
         << dtlsTypeString(recordType) << '(' << static_cast<int>(recordType)
         << ')' << " len=" << recordLen << " epoch=" << recordEpoch
         << " seq=" << recordSeqNum;

  if (recordEpoch > 0) {
    // Data fragment is encrypted when epoch > 0.
    stream << " [encrypted]";

  } else if (recordType == kHandshakeType) {
    if (recordLen >= 25) {
      // Pull apart handshake header.
      //
      //   uint8 msg_type;
      //   uint24 length;
      //   uint16 message_seq;
      //   uint24 fragment_offset;
      //   uint24 fragment_length;

      UInt8 hsType = data[13];
      UInt32 hsLength = (UInt32_cast(data[14]) << 16) |
                        (UInt32_cast(data[15]) << 8) | UInt32_cast(data[16]);
      UInt32 hsMsgSeq = (UInt32_cast(data[17]) << 8) | data[18];
      UInt32 hsFragmentOffset = (UInt32_cast(data[19]) << 16) |
                                (UInt32_cast(data[20]) << 8) |
                                UInt32_cast(data[21]);
      UInt32 hsFragmentLength = (UInt32_cast(data[22]) << 16) |
                                (UInt32_cast(data[23]) << 8) |
                                UInt32_cast(data[24]);
      stream << ' ' << dtlsHandshakeType(hsType) << '('
             << static_cast<int>(hsType) << ')';
      stream << " msgLen=" << hsLength << " msgSeq=" << hsMsgSeq
             << " fragmentOffset=" << hsFragmentOffset;
      stream << " (" << hsFragmentLength
             << " bytes)";  // << RawDataToHex(&data[25], recordLen - 25);

      if (hsType == kHelloVerifyRequest && recordLen >= 28) {
        const UInt8 *cookie = &data[27];
        if (*cookie != recordLen - 28) {
          stream << " [malformed]";
        } else if (cookie[0] > 0) {
          stream << " cookie=" << RawDataToHex(cookie + 1, cookie[0]);
        }
      } else if (hsType == kClientHelloRequest && recordLen >= 61) {
        const UInt8 *cookie = &data[60];
        if (*cookie > recordLen - 61) {
          stream << " [malformed]";
        } else if (cookie[0] > 0) {
          stream << " cookie=" << RawDataToHex(cookie + 1, cookie[0]);
        }
      }
    } else {
      // Handshake header is too short.

      stream << " [truncated]";
    }
  }

  return stream.str();
}
