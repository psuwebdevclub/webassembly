#!/bin/bash

echo "Building N-Body Simulation WebAssembly module..."

# Ensure we have the right permissions
chmod +x "$0"

# Navigate to the directory with the C code
cd "$(dirname "$0")"

# Define output directory
OUTPUT_DIR="../../../public"
mkdir -p $OUTPUT_DIR

# Compile the C code to WebAssembly
emcc n_body_simulation.c \
    -o $OUTPUT_DIR/n_body_simulation.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_init", "_updatePositions", "_getSimulationData", "_getBodyCount", "_setTimeStep", "_getTimeStep", "_malloc", "_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "getValue", "setValue"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864 \
    -s MAXIMUM_MEMORY=268435456 \
    -O3 \
    -s NO_EXIT_RUNTIME=1 

echo "N-Body Simulation WebAssembly module built successfully!"
echo "To run the demo, start a local server (e.g., python3 -m http.server) and navigate to index.html" 