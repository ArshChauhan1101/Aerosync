# AeroSync: Distributed Avionics Telemetry System

AeroSync is a high-integrity, client-server framework designed for the reliable transmission of large-scale bit-mapped telemetry data between an **Aircraft (Pilot/Client)** and a **Ground Station (ATC/Server)** using C++ and the TCP/IP protocol suite.

## 🚀 Key Features
* **Reliable Data Transfer:** Implements a custom application-layer protocol over TCP/IP to ensure 100% data integrity for 1.0 MB bit-mapped objects.
* **Persistent State Machine:** The ATC Server utilizes a continuous Finite State Machine (FSM) that automatically resets to a LISTENING state after every successful mission.
* **12-Byte Header Protocol:** Every transmission begins with a fixed 12-byte metadata header (Type, Seq, Length) to ensure synchronized data parsing.
* **Dynamic Memory Safety:** Uses strict heap allocation (`new`/`delete`) to manage the 1,048,576-byte telemetry buffer without memory leaks.
* **Black Box Auditing:** Real-time, append-only transaction logging to `client_log.txt` and `server_log.txt` for safety-critical regulatory compliance.
* **Blackout Protection:** Integrated 5-second timeout mechanism to detect signal loss and prevent system hangs during transmission.

## 🛠 Project Structure
* `src/server.cpp`: Persistent Ground Station logic, handshake verification, and 1MB partitioning.
* `src/client.cpp`: Aircraft logic, dynamic buffer reassembly, and timeout management.
* `include/constants.h`: Shared definitions for the **12-byte Data Packet** structure and system ports.
* `include/logger.h`: Thread-safe, flush-enabled logging utility for audit trail generation.
* `Makefile`: Automated build script optimized for MacOS (Clang/C++17).

## 📊 Technical Specifications
| Component | Requirement ID | Description |
| :--- | :--- | :--- |
| **Protocol** | REQ-COM-010 | TCP/IP Suite for distributed exchange. |
| **Object Size** | REQ-CLT-030 | 1,048,576 Bytes (1 MB) Bit-mapped Telemetry. |
| **Packet Header**| REQ-PKT-030 | Fixed 12-byte header (Type, Seq, Length). |
| **Security** | REQ-SVR-020 | Formal 3-way handshake verification gate. |
| **Timeout** | REQ-COM-060 | 5-second maximum wait for signal blackout detection. |
| **Logging** | REQ-LOG-060 | Session headers with unique Target IDs and timestamps. |

## ⚙️ Building & Running
This project requires a C++17 compatible compiler (GCC/Clang).

1. **Clone the repository:**
   ```bash
   git clone https://github.com/ArshChauhan1101/Aerosync.git
   cd Aerosync


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