#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger {
private:
    std::ofstream logFile;
    std::string fileName;

    // Get current time for REQ-LOG-010
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

public:
    Logger(std::string name) : fileName(name) {
        // REQ-LOG-020/030: Open in Append mode so we don't delete old data
        logFile.open(fileName, std::ios::app);

        // REQ-LOG-060: Write a Session Header when we start
        if (logFile.is_open()) {
            logFile << "\n--- NEW SESSION START: " << getTimestamp() << " ---" << std::endl;
            logFile << "Target ID: AEROSYNC-DS-01" << std::endl;
            logFile << "------------------------------------------" << std::endl;
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    // REQ-LOG-010/040: Log packet details
    void logPacket(uint32_t type, uint32_t seq, uint32_t size, std::string status) {
        if (logFile.is_open()) {
            logFile << "[" << getTimestamp() << "] "
                << "TYPE: " << type << " | "
                << "SEQ: " << seq << " | "
                << "SIZE: " << size << " bytes | "
                << "STATUS: " << status << std::endl;
            logFile.flush(); // Ensure it writes to disk immediately
        }
    }
};

#endif