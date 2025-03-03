#!/bin/bash

echo "Building WebAssembly demos..."

# Ensure we have the right permissions
chmod +x "$0"

# Build the N-body simulation
echo "Building N-Body Simulation..."
cd src/n-body-simulation/c
./build.sh
cd ../../..

echo "All demos built successfully!"
echo "To run the demos, start a local server (e.g., python3 -m http.server) and navigate to index.html" 