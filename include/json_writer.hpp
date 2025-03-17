#ifndef JSON_WRITER_HPP
#define JSON_WRITER_HPP

#include "packet_parser.hpp"
#include <vector>
#include <string>

void writePacketsToJson(const std::vector<Packet>& packets, const std::string& filename);

#endif