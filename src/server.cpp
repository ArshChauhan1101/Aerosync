#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/constants.h"
#include "../include/logger.h"
#include <vector>
#include <filesystem>

// REQ-SVR-060: Simple State Machine states
enum ServerState { IDLE, LISTENING, CONNECTED, ERROR };

namespace fs = std::filesystem;

// REQ-SVR-040: Create dummy telemetry data
void generateTelemetryFile(const std::string& filename) {
    if (std::filesystem::exists(filename)) return;

    std::ofstream outFile(filename, std::ios::binary);
    if (outFile.is_open()) {
        // We create exactly 1,048,576 bytes (1MB) 
        for (uint32_t i = 0; i < 1048576; i += 64) {
            // 1. Structural Header (8 bytes)
            char frameHeader[8] = {'A', 'E', 'R', 'O', 'S', 'Y', 'N'}; 
            frameHeader[7] = (i / 64) % 256; // Frame ID for tracking

            // 2. Simulated Flight Data (8 bytes)
            float altitude = 30000.0f + (float)(i % 500);
            float airspeed = 450.0f + (float)(i % 50);

            outFile.write(frameHeader, 8);
            outFile.write(reinterpret_cast<char*>(&altitude), sizeof(float));
            outFile.write(reinterpret_cast<char*>(&airspeed), sizeof(float));

            // 3. Padding/Sensor Matrix (48 bytes) to fill the 64-byte frame
            char padding[48];
            for(int j=0; j<48; ++j) padding[j] = (char)(j + 65); // Clear 'A-Z' pattern
            
            outFile.write(padding, 48);
        }
        outFile.close();
        std::cout << "[ATC] Created Structured 1.0 MB Avionics Matrix." << std::endl;
    }
}

int main() {
    generateTelemetryFile("telemetry_data.bin");

    Logger serverLog("server_log.txt"); 
    ServerState currentState = IDLE;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 1. Create socket (REQ-COM-010)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    // 2. Attach socket to the port (REQ-COM-020)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // 3. Start Listening
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    // --- START OF WHILE LOOP ---
    while (true) {
        currentState = LISTENING;
        std::cout << "\n[ATC SERVER] Status: LISTENING on port " << PORT << "..." << std::endl;

        // 4. Accept Connection (REQ-SVR-010)
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            currentState = ERROR;
            continue; // Keep server alive even if one connection fails
        }

        currentState = CONNECTED;
        std::cout << "[ATC SERVER] Status: CONNECTED. Aircraft on link." << std::endl;

        // 5. Receive Handshake Request (REQ-COM-030)
        PacketHeader header;
        recv(new_socket, &header, sizeof(header), 0);

        if (header.packetType == 1) { 
            std::cout << "[ATC SERVER] Handshake Request received (Seq: " << header.sequenceNumber << ")" << std::endl;
            serverLog.logPacket(header.packetType, header.sequenceNumber, 0, "RECEIVED_OK");
            
            PacketHeader ackHeader = {4, 0, 0}; 
            send(new_socket, &ackHeader, sizeof(ackHeader), 0);
            serverLog.logPacket(4, 0, 0, "SENT_ACK");

            std::cout << "[ATC SERVER] Handshake Successful. System OPERATIONAL." << std::endl;
        }

        // 6. Handle Data Request (REQ-SVR-030)
        PacketHeader request;
        recv(new_socket, &request, sizeof(request), 0);

        if (request.packetType == 2) { 
            std::cout << "[ATC] Data Request received. Starting 1MB Stream..." << std::endl;
            serverLog.logPacket(2, 102, 0, "REQUEST_ACCEPTED");

            std::ifstream inFile("telemetry_data.bin", std::ios::binary);
            char buffer[1024]; 
            uint32_t seqNum = 1;
            uint32_t totalSent = 0;

            while (inFile.read(buffer, sizeof(buffer)) || inFile.gcount() > 0) {
                uint32_t bytesRead = inFile.gcount();
                PacketHeader dataHeader = {3, seqNum++, bytesRead};

                send(new_socket, &dataHeader, sizeof(dataHeader), 0);
                send(new_socket, buffer, bytesRead, 0);

                totalSent += bytesRead;
            }
            std::cout << "[ATC] Transfer Complete. Total Bytes Sent: " << totalSent << std::endl;
            serverLog.logPacket(3, seqNum, totalSent, "TRANSFER_COMPLETE");
            inFile.close();
        }

        // Close the client session but keep the listener (server_fd) open
        sleep(1); 
        close(new_socket);
        std::cout << "[ATC SERVER] Session closed. Returning to IDLE/LISTENING." << std::endl;
    }
    // --- END OF WHILE LOOP ---

    close(server_fd);
    return 0;
}