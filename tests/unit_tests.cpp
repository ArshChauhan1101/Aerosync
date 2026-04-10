#include <iostream>
#include <cassert>
#include "../include/constants.h"

// CLT-UT-001: Validate Packet Header Structure (REQ-PKT-030)
void test_packet_structure() {
    PacketHeader testHeader = {1, 101, 1024};
    
    std::cout << "[UNIT TEST] Validating REQ-PKT-030 (12-byte header)..." << std::endl;
    
    // Ensure the struct is exactly 12 bytes
    assert(sizeof(testHeader) == 12); 
    assert(testHeader.packetType == 1);
    assert(testHeader.payloadLength == 1024);
    
    std::cout << "PASS: Header structure is exactly 12 bytes and correctly aligned." << std::endl;
}

// SVR-UT-001: Validate Telemetry Size (REQ-SVR-030)
void test_telemetry_size() {
    std::cout << "[UNIT TEST] Validating REQ-SVR-030 (1MB Constant)..." << std::endl;
    
    // 1024 * 1024 = 1,048,576
    assert(TELEMETRY_SIZE == 1048576);
    
    std::cout << "PASS: Telemetry size constant matches 1.0 MB specification." << std::endl;
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "   AEROSYNC UNIT TEST SUITE (Sprint 2)    " << std::endl;
    std::cout << "==========================================" << std::endl;

    try {
        test_packet_structure();
        test_telemetry_size();
        
        std::cout << "\nALL UNIT TESTS PASSED SUCCESSFULLY." << std::endl;
    } catch (...) {
        std::cout << "\nFAIL: One or more unit tests triggered an exception." << std::endl;
        return -1;
    }

    return 0;
}