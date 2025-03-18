#include "json_writer.hpp"
#include "logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Method to write a collection of packets to a JSON file
void writePacketsToJson(const std::vector<Packet>& packets, const std::string& filename) {
    // Create a JSON array to store packet data
    json j = json::array();

    // Iterate through each packet and serialize its fields into JSON format
    for (const auto& p : packets) {
        j.push_back({
            {"symbol", p.symbol},                   // Store the packet symbol
            {"side", std::string(1, p.side)},       // Store the packet side (converted to string)
            {"quantity", p.quantity},               // Store the packet quantity
            {"price", p.price},                     // Store the packet price
            {"sequence", p.sequence}                // Store the packet sequence number
        });
    }

    // Open the specified output file for writing
    std::ofstream outFile(filename);

    // Check if the file stream is successfully opened
    if (!outFile) {
        LOG_ERROR("Failed to open output file: " + filename);
        return;
    }

    outFile << j.dump(4);

    outFile.close();

    LOG_INFO("Successfully wrote " + std::to_string(packets.size()) + " packets to JSON file: " + filename);
}