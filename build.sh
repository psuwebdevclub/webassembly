#!/bin/bash

echo "Building Fractal Renderer WebAssembly module..."

# Build fractal renderer
cd src/fractal-renderer/c
./build.sh
cd ../../..

echo "WebAssembly module built successfully!"
echo "Run a local server to view the demo."
echo "Example: python3 -m http.server" 