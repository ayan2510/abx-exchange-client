#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client {
public:
    Client(const std::string& host, int port);
    void requestPackets();
    void requestMissingPacket(uint32_t sequence);
    std::vector<uint8_t> receiveData();
    void close();

private:
    boost::asio::io_context io_context;
    tcp::socket socket;
    std::string host;
    int port;

    void sendRequest(uint8_t callType, uint8_t resendSeq = 0);
};

#endif