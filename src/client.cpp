#include "client.hpp"
#include <fstream>
#include <iostream>
#include "logger.hpp"
#include "client.hpp"

// Constructor to initialize the client and establish a connection to the server
Client::Client(const std::string& host, int port)
    : socket(io_context), host(host), port(port) {
    try {
        // Resolve the hostname and port
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));

        // Connect to the server using the resolved endpoints
        boost::asio::connect(socket, endpoints);
        LOG_INFO("Connected to " + host + ":" + std::to_string(port));
    } catch (const std::exception& e) {
        LOG_ERROR("Connection failed: " + std::string(e.what()));
        throw;
    }
}

// Method to reconnect to the server in case of disconnection
void Client::reconnect() {
    try {
        socket.close();  // Close the existing socket

        // Resolve and reconnect to the server
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(socket, endpoints);

        LOG_INFO("Reconnected to " + host + ":" + std::to_string(port));
    } catch (const std::exception& e) {
        LOG_ERROR("Reconnection failed: " + std::string(e.what()));
    }
}

// Method to send a request to the server
void Client::sendRequest(uint8_t requestType, uint8_t sequence) {
    try {
        // Create a request packet containing the request type and sequence number
        std::vector<uint8_t> request{requestType, sequence};

        // Send the request packet through the socket
        boost::asio::write(socket, boost::asio::buffer(request));

        LOG_INFO("Sent request - Type: " + std::to_string(requestType) + ", Sequence: " + std::to_string(sequence));
    } catch (const std::exception& e) {
        LOG_ERROR("Error while sending request: " + std::string(e.what()));
        reconnect();
    }
}

// Method to request all packets from the server
void Client::requestPackets() {
    LOG_INFO("Requesting all packets.");

    // Send a request with type 1 to retrieve all packets
    sendRequest(1);  // Stream All Packets
}

// Method to request a specific missing packet by sequence number
void Client::requestMissingPacket(uint32_t sequence) {
    if (!socket.is_open()) {
        // If the socket is closed, attempt to reconnect
        LOG_INFO("Socket closed. Reconnecting...");
        reconnect();
    }

    LOG_INFO("Requesting missing packet: " + std::to_string(sequence));

    // Send a request with type 2 to retrieve the missing packet
    sendRequest(2, static_cast<uint8_t>(sequence));
}

// Method to receive data from the server
std::vector<uint8_t> Client::receiveData() {
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> tempBuffer(1024);

    try {
        while (true) {
            // Read incoming data into a temporary buffer
            size_t length = socket.read_some(boost::asio::buffer(tempBuffer));

            // Append the received data to the main buffer
            buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + length);
        }
    } catch (const boost::system::system_error& e) {
        // Handle end-of-file or other reading errors
        if (e.code() == boost::asio::error::eof) {
            LOG_INFO("Server closed the connection.");
        } else {
            LOG_ERROR("Error while reading: " + std::string(e.what()));
        }
    }

    LOG_INFO("Received " + std::to_string(buffer.size()) + " bytes of data.");
    return buffer;
}

// Method to receive missing packet data from the server
std::vector<uint8_t> Client::receiveMissingData() {
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> tempBuffer(1024);

    try {
        // Read incoming data for a single missing packet
        size_t length = socket.read_some(boost::asio::buffer(tempBuffer));
        buffer.insert(buffer.end(), tempBuffer.begin(), tempBuffer.begin() + length);
    } catch (const boost::system::system_error& e) {
        // Handle end-of-file or other errors
        if (e.code() == boost::asio::error::eof) {
            LOG_INFO("Server closed the connection.");
        } else {
            LOG_ERROR("Error while reading: " + std::string(e.what()));
        }
    }

    LOG_INFO("Received " + std::to_string(buffer.size()) + " bytes of missing data.");
    return buffer;
}

// Method to close the client socket and end the connection
void Client::close() {
    socket.close();
    LOG_INFO("Socket closed.");
}
