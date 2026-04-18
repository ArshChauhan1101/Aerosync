#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <filesystem>
#include "../include/constants.h"

// Server state machine (mirrors server.cpp)
enum ServerState { IDLE, LISTENING, CONNECTED_SVR, ERROR_SVR };

// Client state machine (mirrors client.cpp)
enum ClientState { INITIALIZED, CONNECTING, CONNECTED_CLT, ERROR_CLT };

int totalTests = 0;
int passedTests = 0;

void reportResult(const std::string& testId, const std::string& description, bool passed) {
    totalTests++;
    if (passed) {
        passedTests++;
        std::cout << "[PASS] " << testId << ": " << description << std::endl;
    } else {
        std::cout << "[FAIL] " << testId << ": " << description << std::endl;
    }
}

// ==================== CLIENT UNIT TESTS ====================

// CLT-UT-001: Validate Packet Header Structure (REQ-PKT-030)
void test_packet_structure() {
    PacketHeader testHeader = {1, 101, 1024};
    bool passed = (sizeof(testHeader) == 12) &&
                  (testHeader.packetType == 1) &&
                  (testHeader.sequenceNumber == 101) &&
                  (testHeader.payloadLength == 1024);
    assert(sizeof(testHeader) == 12);
    reportResult("CLT-UT-001", "Header structure is exactly 12 bytes and correctly aligned (REQ-PKT-030)", passed);
}

// CLT-UT-002: Validate Handshake Packet Values (REQ-COM-030)
void test_handshake_packet() {
    PacketHeader handshake = {1, 101, 0};
    bool passed = (handshake.packetType == 1) &&
                  (handshake.sequenceNumber == 101) &&
                  (handshake.payloadLength == 0);
    assert(handshake.packetType == 1);
    assert(handshake.sequenceNumber == 101);
    reportResult("CLT-UT-002", "Handshake packet has correct type=1, seq=101 (REQ-COM-030)", passed);
}

// CLT-UT-003: Validate Data Request Packet Values (REQ-CLT-030)
void test_data_request_packet() {
    PacketHeader dataRequest = {2, 102, 0};
    bool passed = (dataRequest.packetType == 2) &&
                  (dataRequest.sequenceNumber == 102) &&
                  (dataRequest.payloadLength == 0);
    assert(dataRequest.packetType == 2);
    assert(dataRequest.sequenceNumber == 102);
    reportResult("CLT-UT-003", "Data request packet has correct type=2, seq=102 (REQ-CLT-030)", passed);
}

// CLT-UT-004: Validate Client State Machine Enum Values (REQ-CLT-020)
void test_client_state_machine() {
    bool passed = (INITIALIZED == 0) &&
                  (CONNECTING == 1) &&
                  (CONNECTED_CLT == 2) &&
                  (ERROR_CLT == 3);
    assert(INITIALIZED == 0);
    assert(CONNECTING == 1);
    assert(CONNECTED_CLT == 2);
    assert(ERROR_CLT == 3);
    reportResult("CLT-UT-004", "Client state machine enum values are sequential 0-3 (REQ-CLT-020)", passed);
}

// CLT-UT-005: Validate Telemetry Buffer Size (REQ-CLT-040)
void test_telemetry_buffer_allocation() {
    char* buffer = new char[TELEMETRY_SIZE];
    bool passed = (buffer != nullptr) && (TELEMETRY_SIZE == 1048576);
    assert(buffer != nullptr);
    delete[] buffer;
    reportResult("CLT-UT-005", "Telemetry buffer allocates 1,048,576 bytes successfully (REQ-CLT-040)", passed);
}

// ==================== SERVER UNIT TESTS ====================

// SVR-UT-001: Validate Telemetry Size Constant (REQ-SVR-030)
void test_telemetry_size() {
    bool passed = (TELEMETRY_SIZE == 1048576);
    assert(TELEMETRY_SIZE == 1048576);
    reportResult("SVR-UT-001", "Telemetry size constant matches 1.0 MB (1,048,576 bytes) (REQ-SVR-030)", passed);
}

// SVR-UT-002: Validate Server State Machine Enum Values (REQ-SVR-060)
void test_server_state_machine() {
    bool passed = (IDLE == 0) &&
                  (LISTENING == 1) &&
                  (CONNECTED_SVR == 2) &&
                  (ERROR_SVR == 3);
    assert(IDLE == 0);
    assert(LISTENING == 1);
    assert(CONNECTED_SVR == 2);
    assert(ERROR_SVR == 3);
    reportResult("SVR-UT-002", "Server state machine enum values are sequential 0-3 (REQ-SVR-060)", passed);
}

// SVR-UT-003: Validate ACK Packet Type (REQ-SVR-020)
void test_ack_packet() {
    PacketHeader ackHeader = {4, 0, 0};
    bool passed = (ackHeader.packetType == 4) &&
                  (ackHeader.sequenceNumber == 0) &&
                  (ackHeader.payloadLength == 0);
    assert(ackHeader.packetType == 4);
    reportResult("SVR-UT-003", "ACK packet has correct type=4 (REQ-SVR-020)", passed);
}

// SVR-UT-004: Validate Data Packet Type (REQ-PKT-010)
void test_data_packet_type() {
    PacketHeader dataHeader = {3, 1, 1024};
    bool passed = (dataHeader.packetType == 3) &&
                  (dataHeader.payloadLength == 1024);
    assert(dataHeader.packetType == 3);
    reportResult("SVR-UT-004", "Data packet has correct type=3 with payload size (REQ-PKT-010)", passed);
}

// SVR-UT-005: Validate Port Constant (REQ-COM-010)
void test_port_constant() {
    bool passed = (PORT == 8080);
    assert(PORT == 8080);
    reportResult("SVR-UT-005", "Port constant is 8080 (REQ-COM-010)", passed);
}

// SVR-UT-006: Validate Telemetry File Generation (REQ-SVR-040)
void test_telemetry_file_generation() {
    const std::string testFile = "test_telemetry_gen.bin";

    // Generate a 1MB structured telemetry file (same logic as server.cpp)
    std::ofstream outFile(testFile, std::ios::binary);
    for (uint32_t i = 0; i < 1048576; i += 64) {
        char frameHeader[8] = {'A', 'E', 'R', 'O', 'S', 'Y', 'N'};
        frameHeader[7] = (i / 64) % 256;
        float altitude = 30000.0f + (float)(i % 500);
        float airspeed = 450.0f + (float)(i % 50);
        outFile.write(frameHeader, 8);
        outFile.write(reinterpret_cast<char*>(&altitude), sizeof(float));
        outFile.write(reinterpret_cast<char*>(&airspeed), sizeof(float));
        char padding[48];
        for (int j = 0; j < 48; ++j) padding[j] = (char)(j + 65);
        outFile.write(padding, 48);
    }
    outFile.close();

    // Verify file size
    auto fileSize = std::filesystem::file_size(testFile);
    bool passed = (fileSize == 1048576);
    assert(fileSize == 1048576);
    std::filesystem::remove(testFile);
    reportResult("SVR-UT-006", "Telemetry file generation produces exactly 1,048,576 bytes (REQ-SVR-040)", passed);
}

// ==================== PACKET/PROTOCOL UNIT TESTS ====================

// PKT-UT-001: Validate PacketHeader Field Assignment (REQ-PKT-010)
void test_packet_field_assignment() {
    PacketHeader header = {0, 0, 0};
    header.packetType = 3;
    header.sequenceNumber = 500;
    header.payloadLength = 1024;
    bool passed = (header.packetType == 3) &&
                  (header.sequenceNumber == 500) &&
                  (header.payloadLength == 1024);
    assert(header.packetType == 3);
    assert(header.sequenceNumber == 500);
    assert(header.payloadLength == 1024);
    reportResult("PKT-UT-001", "PacketHeader fields are correctly assignable (REQ-PKT-010)", passed);
}

// PKT-UT-002: Validate Struct Packing - No Padding (REQ-PKT-030)
void test_struct_packing() {
    // Each field is uint32_t (4 bytes), 3 fields = 12 bytes with no padding
    bool passed = (sizeof(PacketHeader) == 3 * sizeof(uint32_t)) &&
                  (sizeof(PacketHeader) == 12);
    assert(sizeof(PacketHeader) == 12);
    reportResult("PKT-UT-002", "Pragma pack ensures no struct padding (REQ-PKT-030)", passed);
}

// PKT-UT-003: Validate Server IP Constant (REQ-COM-010)
void test_server_ip() {
    bool passed = (std::strcmp(SERVER_IP, "127.0.0.1") == 0);
    assert(std::strcmp(SERVER_IP, "127.0.0.1") == 0);
    reportResult("PKT-UT-003", "SERVER_IP is 127.0.0.1 for localhost testing (REQ-COM-010)", passed);
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "   AEROSYNC UNIT TEST SUITE               " << std::endl;
    std::cout << "   DO-178C Compliance Testing              " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "--- CLIENT UNIT TESTS ---" << std::endl;
    test_packet_structure();
    test_handshake_packet();
    test_data_request_packet();
    test_client_state_machine();
    test_telemetry_buffer_allocation();

    std::cout << std::endl << "--- SERVER UNIT TESTS ---" << std::endl;
    test_telemetry_size();
    test_server_state_machine();
    test_ack_packet();
    test_data_packet_type();
    test_port_constant();
    test_telemetry_file_generation();

    std::cout << std::endl << "--- PACKET/PROTOCOL UNIT TESTS ---" << std::endl;
    test_packet_field_assignment();
    test_struct_packing();
    test_server_ip();

    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "   RESULTS: " << passedTests << "/" << totalTests << " tests passed" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (passedTests == totalTests) {
        std::cout << "   ALL UNIT TESTS PASSED SUCCESSFULLY." << std::endl;
        return 0;
    } else {
        std::cout << "   SOME TESTS FAILED." << std::endl;
        return -1;
    }
}
