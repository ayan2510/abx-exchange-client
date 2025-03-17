#include "packet_parser.hpp"
#include <iostream>
#include <algorithm>
#include<unordered_set>

uint32_t bigEndianToUint32(const uint8_t* buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

std::vector<Packet> parsePackets(const std::vector<uint8_t>& data) {
    std::vector<Packet> packets;
    const size_t packetSize = 17;

    for (size_t i = 0; i + packetSize <= data.size(); i += packetSize) {
        Packet p;
        p.symbol = std::string(data.begin() + i, data.begin() + i + 4);
        p.side = static_cast<char>(data[i + 4]);
        p.quantity = bigEndianToUint32(&data[i + 5]);
        p.price = bigEndianToUint32(&data[i + 9]);
        p.sequence = bigEndianToUint32(&data[i + 13]);
        packets.push_back(p);
    }

    return packets;
}

std::set<uint32_t> findMissingSequences(const std::vector<Packet>& packets) {
    std::set<uint32_t> missing;
    if (packets.empty()) return missing;

    std::unordered_set<uint32_t> available_seq;

    uint32_t expected = packets.front().sequence;
    uint32_t max_seq = packets.back().sequence;
    for (const auto& packet : packets) {
        // while (expected < packet.sequence) {
        //     missing.insert(expected++);
        // }
        // expected = packet.sequence + 1;
        available_seq.insert(packet.sequence);
    }

    for (const auto& packet : packets) {
        if (available_seq.find(expected) == available_seq.end()) {
            missing.insert(expected);
        }
    }

    return missing;
}
