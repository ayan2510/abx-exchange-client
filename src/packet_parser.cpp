#include "packet_parser.hpp"
#include "logger.hpp"
#include <iostream>
#include <algorithm>
#include <unordered_set>

// Method to convert 4-byte big-endian buffer to a uint32_t value
uint32_t bigEndianToUint32(const uint8_t* buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

// Method to parse raw byte data into a collection of Packet objects
std::vector<Packet> parsePackets(const std::vector<uint8_t>& data) {
    LOG_INFO("Starting packet parsing. Data size: " + std::to_string(data.size()));

    std::vector<Packet> packets;
    const size_t packetSize = 17; // Each packet is 17 bytes long

    // Loop through the data in chunks of packetSize
    for (size_t i = 0; i + packetSize <= data.size(); i += packetSize) {
        Packet p;

        // Extract packet fields from the byte array
        p.symbol = std::string(data.begin() + i, data.begin() + i + 4); // First 4 bytes: Symbol
        p.side = static_cast<char>(data[i + 4]);                        // 5th byte: Side (char)
        p.quantity = bigEndianToUint32(&data[i + 5]);                    // Next 4 bytes: Quantity (big-endian)
        p.price = bigEndianToUint32(&data[i + 9]);                       // Next 4 bytes: Price (big-endian)
        p.sequence = bigEndianToUint32(&data[i + 13]);                   // Last 4 bytes: Sequence (big-endian)

        // Store parsed packet in the vector
        packets.push_back(p);

        LOG_INFO("Parsed packet: Symbol=" + p.symbol +
            ", Side=" + std::to_string(p.side) +
            ", Quantity=" + std::to_string(p.quantity) +
            ", Price=" + std::to_string(p.price) +
            ", Sequence=" + std::to_string(p.sequence));
    }

    LOG_INFO("Finished packet parsing. Total packets: " + std::to_string(packets.size()));

    return packets;
}

// Method to identify missing sequence numbers in a collection of packets
std::set<uint32_t> findMissingSequences(const std::vector<Packet>& packets) {
    LOG_INFO("Finding missing sequences. Total packets: " + std::to_string(packets.size()));

    std::set<uint32_t> missing; // Stores missing sequence numbers

    // If no packets are available, return an empty set
    if (packets.empty()) {
        LOG_INFO("No packets received. Returning empty missing set.");
        return missing;
    }

    std::unordered_set<uint32_t> available_seq; // Track available sequences

    // Identify the maximum sequence number in the packet collection
    uint32_t max_seq = packets.back().sequence;

    // Store all available sequence numbers in the set
    for (const auto& packet : packets) {
        available_seq.insert(packet.sequence);
    }

    // Check for missing sequence numbers from 1 to max_seq
    for (uint32_t expected = 1; expected < max_seq; expected++) {
        if (available_seq.find(expected) == available_seq.end()) {
            missing.insert(expected); // Add missing sequence to the set
            LOG_INFO("Missing sequence found: " + std::to_string(expected));
        }
    }

    LOG_INFO("Finished finding missing sequences. Total missing: " + std::to_string(missing.size()));

    return missing;
}
