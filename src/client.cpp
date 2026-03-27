#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/constants.h"

// REQ-CLT-020: Client State Machine
enum ClientState { INITIALIZED, CONNECTING, CONNECTED, ERROR };

int main() {
    ClientState currentState = INITIALIZED;
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
    std::cout << "[PILOT AIRCRAFT] Handshake sent. Waiting for ATC response..." << std::endl;

    // 5. Receive ACK
    PacketHeader response;
    recv(sock, &response, sizeof(response), 0);

    if (response.packetType == 4) { // 4 = ACK
        std::cout << "[PILOT AIRCRAFT] Handshake Verified by ATC. Link is stable." << std::endl;
    } else {
        std::cout << "[PILOT AIRCRAFT] Connection Error: No Handshake ACK." << std::endl;
    }


    // Keep link for a moment then close
    sleep(2);
    close(sock);
    
    std::cout << "[PILOT AIRCRAFT] Link closed. Session Ended." << std::endl;
    return 0;
}