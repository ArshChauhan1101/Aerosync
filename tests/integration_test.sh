#!/bin/bash
# AeroSync Integration Test Script
# Validates data communication integrity between client and server (DO-178C Integration Testing)

echo "=========================================="
echo "   AEROSYNC INTEGRATION TEST SUITE"
echo "   DO-178C Integration-Level Verification"
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

# Check log files exist
if [ ! -f "$PROJECT_DIR/client_log.txt" ] || [ ! -f "$PROJECT_DIR/server_log.txt" ]; then
    echo "ERROR: Audit logs missing. Run system_test.sh first."
    exit 1
fi

echo "--- DATA TRANSFER INTEGRITY ---"

# INT-IT-001: Client received exactly 1MB
CLIENT_SIZE=$(grep "REASSEMBLY_SUCCESS" "$PROJECT_DIR/client_log.txt" | tail -n 1 | grep -o 'SIZE: [0-9]*' | awk '{print $2}')
if [ "$CLIENT_SIZE" == "1048576" ]; then
    report "INT-IT-001: Client received exactly 1,048,576 bytes (REQ-CLT-010, REQ-SYS-070)" "PASS"
else
    report "INT-IT-001: Client received exactly 1,048,576 bytes" "FAIL"
    echo "  Found: $CLIENT_SIZE"
fi

# INT-IT-002: Server sent exactly 1MB
SERVER_SIZE=$(grep "TRANSFER_COMPLETE" "$PROJECT_DIR/server_log.txt" | tail -n 1 | grep -o 'SIZE: [0-9]*' | awk '{print $2}')
if [ "$SERVER_SIZE" == "1048576" ]; then
    report "INT-IT-002: Server sent exactly 1,048,576 bytes (REQ-SVR-030, REQ-SYS-070)" "PASS"
else
    report "INT-IT-002: Server sent exactly 1,048,576 bytes" "FAIL"
    echo "  Found: $SERVER_SIZE"
fi

# INT-IT-003: Client and server byte counts match
if [ "$CLIENT_SIZE" == "$SERVER_SIZE" ] && [ -n "$CLIENT_SIZE" ]; then
    report "INT-IT-003: Client/Server byte counts match (REQ-SYS-070)" "PASS"
else
    report "INT-IT-003: Client/Server byte counts match" "FAIL"
fi

echo ""
echo "--- LOG FORMAT VALIDATION ---"

# INT-IT-004: Client log format has required fields (timestamp, TYPE, SEQ, SIZE, STATUS)
if grep -qE '^\[.*\] TYPE: [0-9]+ \| SEQ: [0-9]+ \| SIZE: [0-9]+ bytes \| STATUS:' "$PROJECT_DIR/client_log.txt"; then
    report "INT-IT-004: Client log entries follow required format (REQ-LOG-010, REQ-LOG-040)" "PASS"
else
    report "INT-IT-004: Client log entries follow required format" "FAIL"
fi

# INT-IT-005: Server log format has required fields
if grep -qE '^\[.*\] TYPE: [0-9]+ \| SEQ: [0-9]+ \| SIZE: [0-9]+ bytes \| STATUS:' "$PROJECT_DIR/server_log.txt"; then
    report "INT-IT-005: Server log entries follow required format (REQ-LOG-010, REQ-LOG-040)" "PASS"
else
    report "INT-IT-005: Server log entries follow required format" "FAIL"
fi

echo ""
echo "--- SESSION MANAGEMENT ---"

# INT-IT-006: Session headers present in both logs
if grep -q "NEW SESSION START" "$PROJECT_DIR/client_log.txt" && \
   grep -q "NEW SESSION START" "$PROJECT_DIR/server_log.txt"; then
    report "INT-IT-006: Session headers present in both logs (REQ-LOG-060)" "PASS"
else
    report "INT-IT-006: Session headers present in both logs" "FAIL"
fi

# INT-IT-007: Target ID present in logs
if grep -q "Target ID:" "$PROJECT_DIR/client_log.txt" && \
   grep -q "Target ID:" "$PROJECT_DIR/server_log.txt"; then
    report "INT-IT-007: Target ID recorded in session headers (REQ-LOG-060)" "PASS"
else
    report "INT-IT-007: Target ID recorded in session headers" "FAIL"
fi

echo ""
echo "--- PROTOCOL HANDSHAKE VERIFICATION ---"

# INT-IT-008: Handshake sent by client
if grep -q "SENT_REQUEST" "$PROJECT_DIR/client_log.txt"; then
    report "INT-IT-008: Client sent handshake request (REQ-COM-030)" "PASS"
else
    report "INT-IT-008: Client sent handshake request" "FAIL"
fi

# INT-IT-009: ACK received by client
if grep -q "RECEIVED_ACK" "$PROJECT_DIR/client_log.txt"; then
    report "INT-IT-009: Client received ACK from server (REQ-SVR-020)" "PASS"
else
    report "INT-IT-009: Client received ACK from server" "FAIL"
fi

# INT-IT-010: Data request logged
if grep -q "DATA_REQUEST" "$PROJECT_DIR/client_log.txt"; then
    report "INT-IT-010: Data request logged by client (REQ-CLT-030)" "PASS"
else
    report "INT-IT-010: Data request logged by client" "FAIL"
fi

echo ""
echo "=========================================="
echo "   INTEGRATION TEST RESULTS: $PASS_COUNT passed, $FAIL_COUNT failed"
echo "=========================================="

if [ $FAIL_COUNT -eq 0 ]; then
    echo "   ALL INTEGRATION TESTS PASSED."
    exit 0
else
    echo "   SOME INTEGRATION TESTS FAILED."
    exit 1
fi
