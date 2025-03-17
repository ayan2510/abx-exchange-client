#include "client.hpp"
#include "packet_parser.hpp"
#include "json_writer.hpp"

void printVector(const std::vector<uint8_t>& data) {
    for (auto byte : data) {
        std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;
}

int main() {
    Client client("localhost", 3000);
    client.requestPackets();

    auto data = client.receiveData();


    auto packets = parsePackets(data);
    
    for (auto packet : packets) {
        std::cout << packet.symbol << " " << packet.price << " " << packet.quantity << " " << packet.sequence << " " << packet.side << std::endl;
    }

    auto missingSequences = findMissingSequences(packets);
    for (auto seq : missingSequences) {
        client.requestMissingPacket(seq);
        auto missingData = client.receiveData();
        auto missingPackets = parsePackets(missingData);
        packets.insert(packets.end(), missingPackets.begin(), missingPackets.end());
    }

    writePacketsToJson(packets, "output.json");
    client.close();

    std::cout << "Data saved to output.json\n";
    return 0;
}
