#!/bin/bash

# Set the client directory (adjust if needed)
CLIENT_DIR="$(dirname "$0")"

# Navigate to the client directory
cd "$CLIENT_DIR"

# Remove the 'build' folder if it exists
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

# Create and navigate to the build folder
mkdir build && cd build

# Run CMake and build the client
echo "Building the client..."
cmake .. && make

# Execute the client binary if the build is successful
if [ -f "./abx_client" ]; then
    echo "Running abx_client..."
    ./abx_client
else
    echo "Build failed. abx_client not found."
    exit 1
fi