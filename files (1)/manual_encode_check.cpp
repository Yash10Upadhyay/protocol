// Temporary manual check (not the official round-trip test - that's step 7).
// Encodes a LoginRequest with sample values and prints the resulting bytes
// so the layout can be eyeballed against the schema before writing the
// decoder.
#include <cstdio>
#include <cstring>
#include "../generated/BOE_Msgs.h"
#include "../generated/BOE_Encoder.h"

int main() {
    boe::LoginRequest req{};
    req.SessionSubID = {'A', 'B', 'C', 'D'};
    std::memset(req.Username.data(), 0, req.Username.size());
    std::memcpy(req.Username.data(), "usr1", 4);
    std::memset(req.Password.data(), 0, req.Password.size());
    std::memcpy(req.Password.data(), "pass12345", 9);

    req.UnitSequences.push_back({1, 1000});
    req.UnitSequences.push_back({2, 2000});

    std::uint8_t buffer[128] = {};
    auto result = boe::encode_LoginRequest(req, buffer, sizeof(buffer));

    if (!result.success) {
        std::printf("ENCODE FAILED: %s\n", result.error);
        return 1;
    }

    std::printf("Encoded %zu bytes:\n", result.bytes_written);
    for (std::size_t i = 0; i < result.bytes_written; ++i) {
        std::printf("%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) std::printf("\n");
    }
    std::printf("\n");

    // Spot-check a few known positions against the schema:
    std::printf("\nSpot checks:\n");
    std::printf("  StartOfMessage (bytes 0-1): %02X %02X (expect BA BA)\n", buffer[0], buffer[1]);
    std::printf("  MessageLength  (bytes 2-3): %02X %02X = %u\n", buffer[2], buffer[3],
                 buffer[2] | (buffer[3] << 8));
    std::printf("  MessageType    (byte 4):    %02X (expect 37)\n", buffer[4]);
    std::printf("  Total bytes written should equal 2 + MessageLength: %zu == %u\n",
                 result.bytes_written, 2 + (buffer[2] | (buffer[3] << 8)));

    return 0;
}
