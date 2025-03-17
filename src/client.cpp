#include "client.hpp"

Client::Client(const std::string& host, int port)
    : socket(io_context), host(host), port(port) {
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, std::to_string(port));
    boost::asio::connect(socket, endpoints);
    std::cout << "Connected to " << host << ":" << port << std::endl;
}

void Client::sendRequest(uint8_t callType, uint8_t resendSeq) {
    // std::cout << "Sending request: " << static_cast<int>(callType) << " " 
    //           << static_cast<int>(resendSeq) << std::endl; // Ensure flushing
    std::vector<uint8_t> request = {callType, resendSeq};
    boost::asio::write(socket, boost::asio::buffer(request));
}

void Client::requestPackets() {
    sendRequest(1);  // Stream All Packets
}

void Client::requestMissingPacket(uint32_t sequence) {
    sendRequest(2, static_cast<uint8_t>(sequence));
}

// std::vector<uint8_t> Client::receiveData() {
//     std::vector<uint8_t> buffer(1024);
//     size_t length = socket.read_some(boost::asio::buffer(buffer));
//     buffer.resize(length);
//     return buffer;
// }

std::vector<uint8_t> Client::receiveData() {
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> tempBuffer(1024);

    try {
        while (true) {
            size_t length = socket.read_some(boost::asio::buffer(tempBuffer));
            buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + length);
        }
    } catch (const boost::system::system_error& e) {
        if (e.code() == boost::asio::error::eof) {
            std::cout << "Server closed the connection." << std::endl;
        } else {
            std::cerr << "Error while reading: " << e.what() << std::endl;
        }
    }

    return buffer;
}


void Client::close() {
    socket.close();
}
