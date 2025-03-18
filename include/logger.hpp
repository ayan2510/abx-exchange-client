#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

// Global log file and mutex
extern std::ofstream logFile;
extern std::mutex logMutex;

// Initialize the logger (to be called once at the start)
void initLogger();

// Core logging function
void logInternal(const std::string& message, const char* file, int line, const char* function);

// Helper macro for logging
// #define LOG(message) logInternal(message, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(msg) logInternal(msg, __FILE__, __LINE__, __func__)
#define LOG_ERROR(msg) logInternal(std::string("ERROR: ") + msg, __FILE__, __LINE__, __func__)

#endif
