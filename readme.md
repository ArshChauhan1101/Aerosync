# AeroSync: Distributed Avionics Telemetry System

**CSCN74000 | Group 10 - Arsh Chauhan, Anish Reddy, Yeshoda Prasai**

AeroSync simulates reliable transmission of 1 MB of bit-mapped telemetry data between an Aircraft (Client) and a Ground Station (ATC Server) over TCP/IP. Built in C++17, designed to DO-178C DAL-D standards.

## Project Structure

```
src/server.cpp       - ATC ground station: listens, handshakes, streams 1MB telemetry
src/client.cpp       - Aircraft client: connects, requests, reassembles telemetry
include/constants.h  - 12-byte packet header struct and shared constants
include/logger.h     - Append-only audit logger with timestamps
tests/               - Unit, system, integration, and usability test suites
Makefile             - Build and test automation
```

## Build & Run

Requires a C++17 compiler (GCC/Clang) and GNU Make.

```bash
git clone https://github.com/ArshChauhan1101/Aerosync.git
cd Aerosync
make
```

Open two terminals:

```bash
# Terminal 1
./atc_server

# Terminal 2
./aircraft_client
```

## Testing

```bash
make unit_test        # 14 unit tests
make system_test      # 10 system tests
make integration_test # 10 integration tests
make usability_test   # 8 usability tests
make test             # run all 42 tests

make static_analysis  # cppcheck static analysis
make coverage         # code coverage report
```

All test definitions and execution logs are in `tests/Project_Test_Log.xlsx`.
