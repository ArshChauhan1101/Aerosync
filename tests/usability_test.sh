#!/bin/bash
# AeroSync Usability Test Script
# Validates user-facing output quality and log readability

echo "=========================================="
echo "   AEROSYNC USABILITY TEST SUITE"
echo "   DO-178C Usability Verification"
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

# Build and run to capture console output
cd "$PROJECT_DIR"
make > /dev/null 2>&1

rm -f "$PROJECT_DIR/client_log.txt" "$PROJECT_DIR/server_log.txt" "$PROJECT_DIR/telemetry_data.bin"

./atc_server &
SERVER_PID=$!
sleep 2
CLIENT_OUTPUT=$(./aircraft_client 2>&1)
sleep 3
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "--- CONSOLE OUTPUT CLARITY ---"

# USB-UT-001: Client displays clear status messages
if echo "$CLIENT_OUTPUT" | grep -q "INITIALIZED"; then
    report "USB-UT-001: Client shows INITIALIZED status on startup" "PASS"
else
    report "USB-UT-001: Client shows INITIALIZED status on startup" "FAIL"
fi

# USB-UT-002: Client displays connection status
if echo "$CLIENT_OUTPUT" | grep -q "CONNECTING"; then
    report "USB-UT-002: Client shows CONNECTING status" "PASS"
else
    report "USB-UT-002: Client shows CONNECTING status" "FAIL"
fi

# USB-UT-003: Client displays handshake confirmation
if echo "$CLIENT_OUTPUT" | grep -q "Handshake"; then
    report "USB-UT-003: Client displays handshake status message" "PASS"
else
    report "USB-UT-003: Client displays handshake status message" "FAIL"
fi

# USB-UT-004: Client displays transfer success
if echo "$CLIENT_OUTPUT" | grep -q "SUCCESS"; then
    report "USB-UT-004: Client displays SUCCESS notification on completion" "PASS"
else
    report "USB-UT-004: Client displays SUCCESS notification on completion" "FAIL"
fi

echo ""
echo "--- LOG FILE READABILITY ---"

# USB-UT-005: Client log is human-readable (contains timestamps)
if grep -qE '\[20[0-9]{2}-[0-9]{2}-[0-9]{2}' "$PROJECT_DIR/client_log.txt" 2>/dev/null; then
    report "USB-UT-005: Client log contains human-readable timestamps" "PASS"
else
    report "USB-UT-005: Client log contains human-readable timestamps" "FAIL"
fi

# USB-UT-006: Server log is human-readable (contains timestamps)
if grep -qE '\[20[0-9]{2}-[0-9]{2}-[0-9]{2}' "$PROJECT_DIR/server_log.txt" 2>/dev/null; then
    report "USB-UT-006: Server log contains human-readable timestamps" "PASS"
else
    report "USB-UT-006: Server log contains human-readable timestamps" "FAIL"
fi

# USB-UT-007: Log entries use descriptive status labels
if grep -qE 'STATUS: (SENT_REQUEST|RECEIVED_ACK|DATA_REQUEST|REASSEMBLY_SUCCESS)' "$PROJECT_DIR/client_log.txt" 2>/dev/null; then
    report "USB-UT-007: Log entries use descriptive status labels" "PASS"
else
    report "USB-UT-007: Log entries use descriptive status labels" "FAIL"
fi

# USB-UT-008: Session separator is clearly visible
if grep -q "\-\-\-.*SESSION.*\-\-\-" "$PROJECT_DIR/client_log.txt" 2>/dev/null; then
    report "USB-UT-008: Session boundaries are clearly marked in logs" "PASS"
else
    report "USB-UT-008: Session boundaries are clearly marked in logs" "FAIL"
fi

echo ""
echo "=========================================="
echo "   USABILITY TEST RESULTS: $PASS_COUNT passed, $FAIL_COUNT failed"
echo "=========================================="

if [ $FAIL_COUNT -eq 0 ]; then
    echo "   ALL USABILITY TESTS PASSED."
    exit 0
else
    echo "   SOME USABILITY TESTS FAILED."
    exit 1
fi
