cmake_minimum_required(VERSION 3.10)
project(ABX_Exchange_Client)

set(CMAKE_CXX_STANDARD 17)

# Include the header files (Ensure it points to the "include/" directory)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add all source files, including main.cpp
add_executable(abx_client
    ${CMAKE_SOURCE_DIR}/src/main.cpp         # Ensure this contains the "main()" function
    ${CMAKE_SOURCE_DIR}/src/client.cpp
    ${CMAKE_SOURCE_DIR}/src/packet_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/json_writer.cpp
)

# Link Boost (if required for TCP communication)
find_package(Boost REQUIRED)
target_link_libraries(abx_client Boost::boost)
