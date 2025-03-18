#include "client.hpp"
#include "packet_parser.hpp"
#include "json_writer.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include "logger.hpp"

namespace fs = std::filesystem;

int main() {
    // Initialize the logger to record application logging
    initLogger();
    LOG_INFO("Starting client application");

    try {
        // Ensure the outputs directory exists in the project root
        std::string outputDir = "../outputs";
        if (!fs::exists(outputDir)) {
            fs::create_directory(outputDir);
        }

        // Establish a connection to the server at localhost:3000
        Client client("localhost", 3000);
        LOG_INFO("Connected to server at localhost:3000");
        std::cout << "Connected to server at localhost:3000" << std::endl;

        // Request all available packets from the server
        client.requestPackets();
        LOG_INFO("Requested packets from server");

        // Receive raw data from the server
        auto data = client.receiveData();
        LOG_INFO("Received " + std::to_string(data.size()) + " bytes of data");

        // Parse the received data into structured packet objects
        auto packets = parsePackets(data);
        LOG_INFO("Parsed " + std::to_string(packets.size()) + " packets");

        // Log and display the details of each packet
        for (const auto& packet : packets) {
            LOG_INFO("Packet - Symbol: " + packet.symbol + ", Price: " + std::to_string(packet.price) + ", Quantity: " + std::to_string(packet.quantity) + ", Sequence: " + std::to_string(packet.sequence) + ", Side: " + std::to_string(packet.side));

            std::cout << "Packet - Symbol: " + packet.symbol + ", Price: " + std::to_string(packet.price) + ", Quantity: " + std::to_string(packet.quantity) + ", Sequence: " + std::to_string(packet.sequence) + ", Side: " + std::to_string(packet.side) << std::endl;
        }

        // Close the initial connection to the server
        client.close();
        LOG_INFO("Closed initial connection");
        std::cout << ("Closed initial connection") << std::endl;

        // Identify any missing packet sequences
        auto missingSequences = findMissingSequences(packets);
        LOG_INFO("Found " + std::to_string(missingSequences.size()) + " missing sequences");
        std::cout << "Found " + std::to_string(missingSequences.size()) + " missing sequences" << std::endl;

        // Request and retrieve each missing packet from the server
        for (auto seq : missingSequences) {
            LOG_INFO("Requesting missing packet: " + std::to_string(seq));
            std::cout << "Requesting missing packet: " + std::to_string(seq) << std::endl;

            // Send a request for the missing packet
            client.requestMissingPacket(seq);
            auto missingData = client.receiveMissingData();

            LOG_INFO("Received " + std::to_string(missingData.size()) + " bytes of missing data");
            std::cout << "Received " + std::to_string(missingData.size()) + " bytes of missing data" << std::endl;

            // Parse and append the missing packets to the main packet list
            auto missingPackets = parsePackets(missingData);
            packets.insert(packets.end(), missingPackets.begin(), missingPackets.end());
        }

        // Sort all packets by their sequence number to ensure proper order
        std::sort(packets.begin(), packets.end(), [](const Packet& a, const Packet& b) {
            return a.sequence < b.sequence;
        });

        // Generate timestamp for the output file
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&now_c), "%Y-%m-%d_%H-%M-%S");

        // Create the output filename with timestamp in the outputs folder
        std::string outputFilename = outputDir + "/output_" + timestamp.str() + ".json";

        // Write the complete packet data to a timestamped JSON file
        writePacketsToJson(packets, outputFilename);
        LOG_INFO("Data saved to " + outputFilename);
        std::cout << "Data saved to " + outputFilename << std::endl;

    } catch (const std::exception& e) {
        LOG_ERROR("Exception occurred: " + std::string(e.what()));
        return 1;
    }

    LOG_INFO("Client application finished successfully");
    return 0;
}
