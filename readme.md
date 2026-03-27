# AeroSync: Distributed Avionics Telemetry System

AeroSync is a high-integrity, client-server framework designed for the reliable transmission of large-scale bit-mapped telemetry data between an **Aircraft (Client)** and a **Ground Station (Server)** using C++ and the TCP/IP protocol suite.

## 🚀 Key Features
* **Reliable Data Transfer:** Implements a custom application-layer protocol over TCP/IP to ensure 100% data integrity for 1 MB bit-mapped objects.
* **State Machine Enforcement:** Both Client and Server utilize strict Finite State Machines (FSM) to handle connection handshakes and signal dropouts.
* **Dynamic Memory Management:** Optimized buffer allocation for handling variable-sized telemetry packets without memory leaks.
* **Safety-Critical Logging:** Real-time transaction auditing to `client_log.txt` and `server_log.txt` for regulatory compliance.

## 🛠 Project Structure
* `/src/Server`: Server-side logic, connection listening, and 1MB object partitioning.
* `/src/Client`: Client-side GUI logic, data request triggers, and dynamic reassembly.
* `/include`: Shared headers for the **12-byte Data Packet** structure.
* `/logs`: Persistent audit trail storage.

## 📊 Technical Specifications
| Component | Requirement ID | Description |
| :--- | :--- | :--- |
| **Protocol** | REQ-COM-010 | TCP/IP Suite for distributed exchange. |
| **Object Size** | REQ-CLT-030 | 1,048,576 Bytes (1 MB) Bit-mapped Telemetry. |
| **Packet Header**| REQ-PKT-010 | 12-byte fixed header (Type, Seq, Length). |
| **Security** | REQ-SVR-020 | Formal handshake verification gate. |

## ⚙️ Building the Project
This project requires a C++17 compatible compiler (GCC/MSVC) and a networking library (such as Asio or Winsock2).

1. Clone the repository:
   ```bash
   git clone https://github.com/ArshChauhan1101/Aerosync.git
   ```

2. Run the ATC Server (Terminal 1):
   ```bash
   ./atc_server
   ```

3. Run the Aircraft Client (Terminal 2):
   ```bash
   ./aircraft_client
   ```

4. Clean build files:
   ```bash
   make clean
   ```