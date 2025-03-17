#include "json_writer.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void writePacketsToJson(const std::vector<Packet>& packets, const std::string& filename) {
    json j = json::array();

    for (const auto& p : packets) {
        j.push_back({
            {"symbol", p.symbol},
            {"side", std::string(1, p.side)},
            {"quantity", p.quantity},
            {"price", p.price},
            {"sequence", p.sequence}
        });
    }

    std::ofstream outFile(filename);
    outFile << j.dump(4);
    outFile.close();
}
