#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/logger.h"
#include "../include/constants.h"

// REQ-CLT-020: Client State Machine
enum ClientState { INITIALIZED, CONNECTING, CONNECTED, ERROR };

int main() {
    ClientState currentState = INITIALIZED;

    Logger clientLog("client_log.txt"); // Initialize REQ-CLT-050

    int sock = 0;
    struct sockaddr_in serv_addr;

    std::cout << "[PILOT AIRCRAFT] Status: INITIALIZED. Preparing link..." << std::endl;

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    currentState = CONNECTING;
    std::cout << "[PILOT AIRCRAFT] Status: CONNECTING to ATC at " << SERVER_IP << "..." << std::endl;

    // REQ-COM-060: Implement 5-second timeout mechanism
    struct timeval tv;
    tv.tv_sec = 5;  // 5 seconds 
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Timeout setup failed" << std::endl;
    }

    // 3. Connect to Server (REQ-CLT-020)
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        currentState = ERROR;
        return -1;
    }

    currentState = CONNECTED;

    // 4. Send Handshake Request (REQ-COM-030)
    PacketHeader handshake = {1, 101, 0}; // Type 1, Seq 101
    send(sock, &handshake, sizeof(handshake), 0);
    clientLog.logPacket(1, 101, 0, "SENT_REQUEST"); // ✅ ADDED

    std::cout << "[PILOT AIRCRAFT] Handshake sent. Waiting for ATC response..." << std::endl;

    // 5. Receive ACK
    PacketHeader response;
    recv(sock, &response, sizeof(response), 0);

    if (response.packetType == 4) { // 4 = ACK
        clientLog.logPacket(4, 0, 0, "RECEIVED_ACK"); // ✅ ADDED
        std::cout << "[PILOT AIRCRAFT] Handshake Verified by ATC. Link is stable." << std::endl;
    } else {
        clientLog.logPacket(response.packetType, response.sequenceNumber, 0, "INVALID_ACK"); // ✅ OPTIONAL
        std::cout << "[PILOT AIRCRAFT] Connection Error: No Handshake ACK." << std::endl;
    }

    // 6. Request 1MB Telemetry (REQ-CLT-030)
    PacketHeader dataRequest = {2, 102, 0}; // Type 2 = Request
    send(sock, &dataRequest, sizeof(dataRequest), 0);
    clientLog.logPacket(2, 102, 0, "DATA_REQUEST_SENT");
    std::cout << "[PILOT] Requesting 1MB Telemetry Object..." << std::endl;

    // 7. Receive and Reassemble (REQ-CLT-040)
    uint32_t totalReceived = 0;
    char* telemetryBuffer = new char[TELEMETRY_SIZE]; // Dynamic Memory Allocation

    std::cout << "[PILOT] Receiving data stream..." << std::endl;

while (totalReceived < TELEMETRY_SIZE) {
    PacketHeader chunkHeader;
    recv(sock, &chunkHeader, sizeof(chunkHeader), 0);

    if (chunkHeader.packetType == 3) {
        // Receive the actual payload
        int bytes = recv(sock, telemetryBuffer + totalReceived, chunkHeader.payloadLength, 0);
        totalReceived += bytes;
    }
}

    // 8. Final Notification (REQ-CLT-010)
    if (totalReceived == TELEMETRY_SIZE) {
        std::cout << "[PILOT] SUCCESS: 1.0 MB Telemetry Object Reassembled!" << std::endl;
        clientLog.logPacket(3, 0, totalReceived, "REASSEMBLY_SUCCESS");
    }

    // Don't forget to clean up memory!
    delete[] telemetryBuffer;

    // Keep link for a moment then close
    sleep(2);
    close(sock);
    
    std::cout << "[PILOT AIRCRAFT] Link closed. Session Ended." << std::endl;
    return 0;
}
