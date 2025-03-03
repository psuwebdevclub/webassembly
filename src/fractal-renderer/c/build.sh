#!/bin/bash

# Create output directory if it doesn't exist
mkdir -p ../../../public

# Compile the C code to WebAssembly
emcc -O3 fractal_renderer.c -o ../../../public/fractal_renderer_wasm.js \
    -s WASM=1 \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","addFunction","UTF8ToString","setValue","getValue","HEAPU8"]' \
    -s EXPORTED_FUNCTIONS='["_init", "_setView", "_renderFractal", "_renderFractalRegion", "_getOutputBuffer", "_moveView", "_zoomView", "_getCenterX", "_getCenterY", "_getZoom", "_malloc", "_free"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864 \
    -s MAXIMUM_MEMORY=134217728 \
    -s ASSERTIONS=1 \
    -s TOTAL_STACK=33554432 \
    -s MODULARIZE=1 \
    -s EXPORT_ES6=0 \
    -s EXPORT_NAME="FractalRendererModule"

echo "WebAssembly module compiled successfully." 