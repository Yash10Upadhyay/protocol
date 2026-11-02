// ---------------------------------------------------------------------------
// EncodeUtils.h — hand-written runtime support for generated encoders.
// This file is NOT generated. It's shared, stable infrastructure that
// every generated encoder #includes.
//
// Byte order note: BOE is little-endian on the wire. Rather than
// reinterpret_cast + memcpy an integer directly (which silently assumes
// the host CPU is also little-endian), each multi-byte writer shifts
// and masks explicitly. This costs nothing at -O2 (the compiler folds
// it back into a single mov on little-endian targets) and stays correct
// if this code ever runs on a big-endian host.
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace boe {

struct EncodeResult {
    bool success;
    std::size_t bytes_written;
    const char* error;
};

inline void write_u8(std::uint8_t* buf, std::size_t offset, std::uint8_t v) {
    buf[offset] = v;
}

inline void write_u16_le(std::uint8_t* buf, std::size_t offset, std::uint16_t v) {
    buf[offset]     = static_cast<std::uint8_t>(v & 0xFF);
    buf[offset + 1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
}

inline void write_u32_le(std::uint8_t* buf, std::size_t offset, std::uint32_t v) {
    buf[offset]     = static_cast<std::uint8_t>(v & 0xFF);
    buf[offset + 1] = static_cast<std::uint8_t>((v >> 8) & 0xFF);
    buf[offset + 2] = static_cast<std::uint8_t>((v >> 16) & 0xFF);
    buf[offset + 3] = static_cast<std::uint8_t>((v >> 24) & 0xFF);
}

inline void write_fixed_string(std::uint8_t* buf, std::size_t offset,
                                const char* data, std::size_t len) {
    std::memcpy(buf + offset, data, len);
}

} // namespace boe
