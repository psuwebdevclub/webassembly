# Mandelbrot Fractal Renderer - WebAssembly vs JavaScript

This project showcases the performance differences between WebAssembly and JavaScript through a visually impressive Mandelbrot fractal renderer. The demo implements the same functionality in both WebAssembly (compiled from C) and pure JavaScript, allowing you to toggle between them to see the performance difference in real-time.

## Demo

The **Fractal Renderer** is a real-time Mandelbrot fractal explorer with smooth zooming and panning. It allows you to:

- Click and drag to move around the fractal
- Use the mouse wheel to zoom in and out
- Double-click to center on a specific point
- Toggle between WebAssembly and JavaScript implementations to compare performance
- Jump to interesting fractal locations with the preset buttons

## Running the Demo

### Prerequisites

- Emscripten SDK installed and available on your PATH
- A modern browser with WebAssembly support
- A local development server

### Setup

1. Clone this repository:
```
git clone https://github.com/yourusername/mandelbrot-wasm.git
cd mandelbrot-wasm
```

2. Compile the WebAssembly module:
```
./build.sh
```

3. Start a local development server. You can use Python's built-in HTTP server:
```
python3 -m http.server
```
Or if you have Node.js installed:
```
npx serve
```

4. Open your browser and navigate to `http://localhost:8000`

## Performance Comparison

The demo includes real-time performance metrics:

- **Render Time** - How long each fractal render takes to compute
- **FPS** - Frames per second
- **Toggle Switch** - Switch between WebAssembly and JavaScript implementations

## Technology

- **C** - The native code compiled to WebAssembly
- **Emscripten** - The compiler toolchain to convert C to WebAssembly
- **JavaScript** - The pure JS implementation for comparison
- **HTML5 Canvas** - For rendering the visual output

## How it Works

The Mandelbrot set is a perfect showcase for WebAssembly's performance advantages for computationally intensive tasks. Each pixel requires an iterative calculation to determine if it's in the set, and WebAssembly modules are compiled ahead of time, allowing for optimizations that aren't possible with JIT-compiled JavaScript.

The performance difference is especially noticeable when zooming into complex areas of the fractal, where the WebAssembly version maintains higher frame rates and lower render times.

## License

MIT License 