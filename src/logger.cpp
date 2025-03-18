#include "logger.hpp"
#include <filesystem>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Define global log file and mutex
std::ofstream logFile;
std::mutex logMutex;


// Create or open the log file with timestamp
void initLogger() {
    // Set log folder relative to the project root
    std::string logFolder = "../log";
    std::filesystem::create_directories(logFolder);

    // Generate timestamp for the log file
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H-%M-%S");

    // Create a timestamped filename
    std::string logFilePath = logFolder + "/client_log_" + timestamp.str() + ".log";

    // Open log file in append mode
    logFile.open(logFilePath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error: Unable to open log file: " << logFilePath << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "Logging initialized. Log file: " << logFilePath << std::endl;
}


// Helper to get current timestamp
std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_r(&now, &localTime);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return std::string(buffer);
}

// Core logging function
void logInternal(const std::string& message, const char* file, int line, const char* function) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::string timestamp = getCurrentTimestamp();
    std::ostringstream logMessage;
    logMessage << "[" << timestamp << "] (" << file << ":" << line << " in " << function << ") " << message;

    // Write to log file and console
    if (logFile.is_open()) {
        logFile << logMessage.str() << std::endl;
    }
    // std::cout << logMessage.str() << std::endl;
}
