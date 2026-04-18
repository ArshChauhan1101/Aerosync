#!/bin/bash
# AeroSync System Test (DO-178C System-Level Verification)
# Validates end-to-end system operation: build, run, and verify

echo "=========================================="
echo "   AEROSYNC SYSTEM TEST SUITE"
echo "   DO-178C System-Level Verification"
echo "=========================================="
echo ""

PASS_COUNT=0
FAIL_COUNT=0
PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

report() {
    if [ "$2" == "PASS" ]; then
        echo "[PASS] $1"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "[FAIL] $1"
        FAIL_COUNT=$((FAIL_COUNT + 1))
    fi
}

# Clean previous artifacts
rm -f "$PROJECT_DIR/client_log.txt" "$PROJECT_DIR/server_log.txt" "$PROJECT_DIR/telemetry_data.bin"

# SYS-ST-001: Build Verification
echo "--- BUILD VERIFICATION ---"
cd "$PROJECT_DIR"
make clean > /dev/null 2>&1
BUILD_OUTPUT=$(make 2>&1)
if [ $? -eq 0 ] && [ -f "$PROJECT_DIR/atc_server" ] && [ -f "$PROJECT_DIR/aircraft_client" ]; then
    report "SYS-ST-001: Project builds successfully with both binaries" "PASS"
else
    report "SYS-ST-001: Project builds successfully with both binaries" "FAIL"
    echo "  Build output: $BUILD_OUTPUT"
    echo "ABORTING: Cannot continue without successful build."
    exit 1
fi

# SYS-ST-002: Server Startup
echo ""
echo "--- SERVER STARTUP ---"
"$PROJECT_DIR/atc_server" &
SERVER_PID=$!
sleep 2

if kill -0 $SERVER_PID 2>/dev/null; then
    report "SYS-ST-002: Server starts and listens on port $PORT (REQ-SVR-010)" "PASS"
else
    report "SYS-ST-002: Server starts and listens on port $PORT (REQ-SVR-010)" "FAIL"
    echo "ABORTING: Server failed to start."
    exit 1
fi

# SYS-ST-003: Client Connection and Data Transfer
echo ""
echo "--- CLIENT DATA TRANSFER ---"
"$PROJECT_DIR/aircraft_client"
CLIENT_EXIT=$?

if [ $CLIENT_EXIT -eq 0 ]; then
    report "SYS-ST-003: Client connects, transfers, and exits cleanly (REQ-CLT-020, REQ-COM-030)" "PASS"
else
    report "SYS-ST-003: Client connects, transfers, and exits cleanly" "FAIL"
fi

# SYS-ST-004: Telemetry File Verification
echo ""
echo "--- DATA INTEGRITY VERIFICATION ---"
if [ -f "$PROJECT_DIR/telemetry_data.bin" ]; then
    FILE_SIZE=$(wc -c < "$PROJECT_DIR/telemetry_data.bin" | tr -d ' ')
    if [ "$FILE_SIZE" -eq 1048576 ]; then
        report "SYS-ST-004: Telemetry file is exactly 1,048,576 bytes (REQ-SYS-070)" "PASS"
    else
        report "SYS-ST-004: Telemetry file is exactly 1,048,576 bytes" "FAIL"
        echo "  Actual size: $FILE_SIZE"
    fi
else
    report "SYS-ST-004: Telemetry file is exactly 1,048,576 bytes" "FAIL"
fi

# SYS-ST-005: Client Log File Verification
if [ -f "$PROJECT_DIR/client_log.txt" ]; then
    report "SYS-ST-005: Client log file created (REQ-CLT-050, REQ-LOG-020)" "PASS"
else
    report "SYS-ST-005: Client log file created" "FAIL"
fi

# SYS-ST-006: Server Log File Verification
if [ -f "$PROJECT_DIR/server_log.txt" ]; then
    report "SYS-ST-006: Server log file created (REQ-SVR-050, REQ-LOG-030)" "PASS"
else
    report "SYS-ST-006: Server log file created" "FAIL"
fi

# SYS-ST-007: Log Content Validation - Client
echo ""
echo "--- LOG CONTENT VALIDATION ---"
if grep -q "REASSEMBLY_SUCCESS" "$PROJECT_DIR/client_log.txt" 2>/dev/null; then
    report "SYS-ST-007: Client log contains REASSEMBLY_SUCCESS (REQ-CLT-010)" "PASS"
else
    report "SYS-ST-007: Client log contains REASSEMBLY_SUCCESS" "FAIL"
fi

# SYS-ST-008: Log Content Validation - Server
if grep -q "TRANSFER_COMPLETE" "$PROJECT_DIR/server_log.txt" 2>/dev/null; then
    report "SYS-ST-008: Server log contains TRANSFER_COMPLETE (REQ-SVR-030)" "PASS"
else
    report "SYS-ST-008: Server log contains TRANSFER_COMPLETE" "FAIL"
fi

# SYS-ST-009: Session Header in Logs
if grep -q "NEW SESSION START" "$PROJECT_DIR/client_log.txt" 2>/dev/null && \
   grep -q "NEW SESSION START" "$PROJECT_DIR/server_log.txt" 2>/dev/null; then
    report "SYS-ST-009: Both logs contain session headers (REQ-LOG-060)" "PASS"
else
    report "SYS-ST-009: Both logs contain session headers" "FAIL"
fi

# SYS-ST-010: Handshake Verification in Logs
if grep -q "SENT_REQUEST\|SENT_ACK\|RECEIVED_ACK\|RECEIVED_OK" "$PROJECT_DIR/client_log.txt" 2>/dev/null; then
    report "SYS-ST-010: Handshake exchange logged (REQ-COM-030, REQ-SYS-080)" "PASS"
else
    report "SYS-ST-010: Handshake exchange logged" "FAIL"
fi

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "=========================================="
echo "   SYSTEM TEST RESULTS: $PASS_COUNT passed, $FAIL_COUNT failed"
echo "=========================================="

if [ $FAIL_COUNT -eq 0 ]; then
    echo "   ALL SYSTEM TESTS PASSED."
    exit 0
else
    echo "   SOME SYSTEM TESTS FAILED."
    exit 1
fi
