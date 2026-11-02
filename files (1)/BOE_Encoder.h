// ---------------------------------------------------------------------------
// AUTO-GENERATED from boe_schema.yaml - DO NOT EDIT BY HAND
// Protocol: BOE / CBOE_OPTIONS  (schema v1.0-trimmed)
// Regenerate with: python generator/render_encoder.py
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>

#include "BOE_Msgs.h"
#include "../common/EncodeUtils.h"

namespace boe {

// ---------------------------------------------------------------------------
// encode_LoginRequest - serializes msg into buffer.
// Returns success=false (without writing anything) if buffer is too
// small; never partially writes a message.
// ---------------------------------------------------------------------------
inline EncodeResult encode_LoginRequest(const LoginRequest& msg,
                                           std::uint8_t* buffer,
                                           std::size_t buffer_size) {
    const std::size_t required_size = 29
        + msg.UnitSequences.size() * 5
    ;
    if (buffer_size < required_size) {
        return EncodeResult{false, 0, "buffer too small for LoginRequest"};
    }

    std::size_t offset = 0;
    std::size_t message_length_offset = 0;

    write_u16_le(buffer, offset, LoginRequest::StartOfMessage);
    offset += 2;
    message_length_offset = offset;   // MessageLength: patched after full message is written
    offset += 2;
    write_u8(buffer, offset, LoginRequest::MessageType);
    offset += 1;
    write_u8(buffer, offset, LoginRequest::MatchingUnit);
    offset += 1;
    write_u32_le(buffer, offset, LoginRequest::SequenceNumber);
    offset += 4;
    write_fixed_string(buffer, offset, msg.SessionSubID.data(), 4);
    offset += 4;
    write_fixed_string(buffer, offset, msg.Username.data(), 4);
    offset += 4;
    write_fixed_string(buffer, offset, msg.Password.data(), 10);
    offset += 10;
    // Derived from the group's actual size, not the (possibly stale) stored field.
    write_u8(buffer, offset, static_cast<std::uint8_t>(msg.UnitSequences.size()));
    offset += 1;
    for (const auto& entry : msg.UnitSequences) {
        write_u8(buffer, offset, entry.UnitNumber);
        offset += 1;
        write_u32_le(buffer, offset, entry.UnitSequenceNumber);
        offset += 4;
    }
    const std::uint16_t message_length = static_cast<std::uint16_t>(offset - 2);
    write_u16_le(buffer, message_length_offset, message_length);

    return EncodeResult{true, offset, nullptr};
}

// ---------------------------------------------------------------------------
// encode_LoginResponse - serializes msg into buffer.
// Returns success=false (without writing anything) if buffer is too
// small; never partially writes a message.
// ---------------------------------------------------------------------------
inline EncodeResult encode_LoginResponse(const LoginResponse& msg,
                                           std::uint8_t* buffer,
                                           std::size_t buffer_size) {
    const std::size_t required_size = 72
        + msg.UnitSequences.size() * 5
    ;
    if (buffer_size < required_size) {
        return EncodeResult{false, 0, "buffer too small for LoginResponse"};
    }

    std::size_t offset = 0;
    std::size_t message_length_offset = 0;

    write_u16_le(buffer, offset, LoginResponse::StartOfMessage);
    offset += 2;
    message_length_offset = offset;   // MessageLength: patched after full message is written
    offset += 2;
    write_u8(buffer, offset, LoginResponse::MessageType);
    offset += 1;
    write_u8(buffer, offset, LoginResponse::MatchingUnit);
    offset += 1;
    write_u32_le(buffer, offset, LoginResponse::SequenceNumber);
    offset += 4;
    buffer[offset] = static_cast<char>(msg.LoginResponseStatus);
    offset += 1;
    write_fixed_string(buffer, offset, msg.LoginResponseText.data(), 60);
    offset += 60;
    // Derived from the group's actual size, not the (possibly stale) stored field.
    write_u8(buffer, offset, static_cast<std::uint8_t>(msg.UnitSequences.size()));
    offset += 1;
    for (const auto& entry : msg.UnitSequences) {
        write_u8(buffer, offset, entry.UnitNumber);
        offset += 1;
        write_u32_le(buffer, offset, entry.UnitSequenceNumber);
        offset += 4;
    }
    const std::uint16_t message_length = static_cast<std::uint16_t>(offset - 2);
    write_u16_le(buffer, message_length_offset, message_length);

    return EncodeResult{true, offset, nullptr};
}

} // namespace boe