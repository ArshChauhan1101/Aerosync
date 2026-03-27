#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// REQ-PKT-030: Fixed 12-byte header structure
#pragma pack(push, 1) // Ensures exactly 12 bytes (no padding)
struct PacketHeader {
    uint32_t packetType;     // 1 = Handshake, 2 = Data Request, 3 = Data, 4 = ACK
    uint32_t sequenceNumber; // Tracking for US-COM-005
    uint32_t payloadLength;  // Size of the data following this header
};
#pragma pack(pop)

// Common Settings
const int PORT = 8080;
const char* SERVER_IP = "127.0.0.1"; // Localhost for testing
const uint32_t TELEMETRY_SIZE = 1048576; // 1 MB (REQ-CLT-030)

#endif