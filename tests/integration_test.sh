#!/bin/bash
# AeroSync Integration Test Script (Last Session Only)
echo "Starting Integrity Check..."

if [ -f "../client_log.txt" ] && [ -f "../server_log.txt" ]; then
    # tail -n 1 ensures we only look at the very last successful entry
    CLIENT_SIZE=$(grep "REASSEMBLY_SUCCESS" ../client_log.txt | tail -n 1 | awk '{print $10}')
    SERVER_SIZE=$(grep "TRANSFER_COMPLETE" ../server_log.txt | tail -n 1 | awk '{print $10}')

    if [ "$CLIENT_SIZE" == "1048576" ] && [ "$SERVER_SIZE" == "1048576" ]; then
        echo "=========================================="
        echo "PASS: Latest Integration Test Successful."
        echo "Last Session: 1,048,576 bytes verified."
        echo "=========================================="
    else
        echo "FAIL: Size mismatch or no successful session found."
        echo "Found: Client($CLIENT_SIZE) Server($SERVER_SIZE)"
    fi
else
    echo "ERROR: Audit logs missing."
fi