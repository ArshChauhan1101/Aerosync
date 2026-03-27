#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/constants.h"

// REQ-SVR-060: Simple State Machine states
enum ServerState { IDLE, LISTENING, CONNECTED, ERROR };

int main() {
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

    currentState = LISTENING;
    std::cout << "[ATC SERVER] Status: LISTENING on port " << PORT << "..." << std::endl;

    // 4. Accept Connection (REQ-SVR-010)
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accept failed" << std::endl;
        currentState = ERROR;
        return -1;
    }

    currentState = CONNECTED;
    // 5. Receive Handshake Request (REQ-COM-030)
    PacketHeader header;
    recv(new_socket, &header, sizeof(header), 0);

    if (header.packetType == 1) { // 1 = Handshake
        std::cout << "[ATC SERVER] Handshake Request received (Seq: " << header.sequenceNumber << ")" << std::endl;
        
        // Send ACK back
        PacketHeader ackHeader = {4, 0, 0}; // 4 = ACK
        send(new_socket, &ackHeader, sizeof(ackHeader), 0);
        std::cout << "[ATC SERVER] Handshake Successful. System OPERATIONAL." << std::endl;
    } else {
        std::cout << "[ATC SERVER] Error: Invalid Handshake. Closing link." << std::endl;
    }

    // Keep connection open for a moment then close (we will add handshake next)
    sleep(2);
    close(new_socket);
    close(server_fd);
    
    std::cout << "[ATC SERVER] Connection closed. Returning to IDLE." << std::endl;
    return 0;
}