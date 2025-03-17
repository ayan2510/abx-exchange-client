#ifndef PACKET_PARSER_HPP
#define PACKET_PARSER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <set>

struct Packet {
    std::string symbol;
    char side;
    uint32_t quantity;
    uint32_t price;
    uint32_t sequence;
};

std::vector<Packet> parsePackets(const std::vector<uint8_t>& data);
std::set<uint32_t> findMissingSequences(const std::vector<Packet>& packets);

uint32_t bigEndianToUint32(const uint8_t* buffer);

#endif