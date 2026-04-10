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
* `tests/`: To test code based on unit and integration of system.
* `Makefile`: Automated build script optimized for MacOS (Clang/C++17).


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


## Testing (WIP)
AeroSync utilizes a multi-tiered testing strategy in accordance with REQ-SYS-020

Next Phase (Weeks 13-14): Expansion of test suite to include full MISRA compliance checks, edge-case payload stress tests, and formal usability trials.

### 1. Unit Testing (REQ-SYS-020)
Individual modules are tested for boundary conditions and structural integrity.
- Compile Tests: g++ -std=c++17 tests/unit_tests.cpp -I ./include -o tests/run_units
- Execute: ./tests/run_units
- Validates: 12-byte header alignment (REQ-PKT-030) and telemetry constants (REQ-SVR-0-30).

### 2. Integration & System Testing (REQ-SYS-070)
Verifies the end-to-end data flow and "bit-perfect" reassembly of the 1.0 MB telemetry object.
- Run the ATC Server and Aircraft Client to complete a transfer.
- Execute Script: ./tests/integration_test.sh
- Validation: The script parses client_log.txt and server_log.txt to verify exactly 1,048,576 bytes were exchanged.