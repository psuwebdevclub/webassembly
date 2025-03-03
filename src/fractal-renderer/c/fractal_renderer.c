#include <stdlib.h>
#include <math.h>
#include <emscripten.h>

// Maximum iterations for Mandelbrot computation
#define MAX_ITERATIONS 1000

// Canvas dimensions
int width, height;

// Viewport parameters - explicitly export these
EMSCRIPTEN_KEEPALIVE double centerX = -0.7;
EMSCRIPTEN_KEEPALIVE double centerY = 0.0;
EMSCRIPTEN_KEEPALIVE double zoom = 1.0;

// Color palette
#define PALETTE_SIZE 256
unsigned char palette[PALETTE_SIZE * 3];

// Output buffer
unsigned char* outputBuffer = NULL;

// Initialize the fractal renderer
EMSCRIPTEN_KEEPALIVE
void init(int canvasWidth, int canvasHeight) {
    width = canvasWidth;
    height = canvasHeight;
    
    // Free previous buffer if it exists
    if (outputBuffer != NULL) {
        free(outputBuffer);
    }
    
    // Allocate buffer for output (RGBA format)
    outputBuffer = (unsigned char*)malloc(width * height * 4);
    
    // Initialize color palette (smooth gradient)
    for (int i = 0; i < PALETTE_SIZE; i++) {
        // Create a visually pleasing color palette
        double t = (double)i / PALETTE_SIZE;
        
        // Use a combination of sine waves for a smooth colorful gradient
        double r = sin(t * 5.0) * 0.5 + 0.5;
        double g = sin(t * 3.0 + 2.0) * 0.5 + 0.5;
        double b = sin(t * 9.0 + 1.0) * 0.5 + 0.5;
        
        palette[i * 3 + 0] = (unsigned char)(r * 255);
        palette[i * 3 + 1] = (unsigned char)(g * 255);
        palette[i * 3 + 2] = (unsigned char)(b * 255);
    }
}

// Set the view parameters
EMSCRIPTEN_KEEPALIVE
void setView(double x, double y, double zoomFactor) {
    centerX = x;
    centerY = y;
    zoom = zoomFactor;
}

// Map iteration count to a smooth color value
double smoothColor(int iterations, double x, double y) {
    if (iterations == MAX_ITERATIONS) {
        return 0.0;
    }
    
    // Smooth coloring algorithm using the log
    double zn = sqrt(x*x + y*y);
    double nu = log(log(zn) / log(2.0)) / log(2.0);
    return (double)(iterations + 1 - nu);
}

// Compute Mandelbrot set for a given pixel
void computePixel(int px, int py, unsigned char* output) {
    // Map pixel coordinates to complex plane
    double aspectRatio = (double)width / height;
    double scale = 3.0 / (zoom * height);
    
    double x0 = (px - width/2) * scale * aspectRatio + centerX;
    double y0 = (py - height/2) * scale + centerY;
    
    // Mandelbrot iteration variables
    double x = 0.0;
    double y = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    int iteration = 0;
    
    // Cardioid and period-2 bulb check (optimization)
    double q = (x0 - 0.25) * (x0 - 0.25) + y0 * y0;
    if (q * (q + (x0 - 0.25)) < 0.25 * y0 * y0 || 
        (x0 + 1.0) * (x0 + 1.0) + y0 * y0 < 0.0625) {
        // Inside the main cardioid or period-2 bulb
        iteration = MAX_ITERATIONS;
    } else {
        // Iterate the Mandelbrot formula: z = z^2 + c
        while (x2 + y2 < 4.0 && iteration < MAX_ITERATIONS) {
            y = 2 * x * y + y0;
            x = x2 - y2 + x0;
            x2 = x * x;
            y2 = y * y;
            iteration++;
        }
    }
    
    // Apply color mapping
    if (iteration == MAX_ITERATIONS) {
        // Inside the set - black
        output[0] = 0;
        output[1] = 0;
        output[2] = 0;
        output[3] = 255; // Alpha
    } else {
        // Calculate smooth color value
        double colorValue = smoothColor(iteration, x, y);
        
        // Map to palette
        int colorIndex = (int)(colorValue * 10.0) % PALETTE_SIZE;
        output[0] = palette[colorIndex * 3 + 0];
        output[1] = palette[colorIndex * 3 + 1];
        output[2] = palette[colorIndex * 3 + 2];
        output[3] = 255; // Alpha
    }
}

// Render the entire fractal
EMSCRIPTEN_KEEPALIVE
void renderFractal() {
    // Compute each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char* pixelData = &outputBuffer[(y * width + x) * 4];
            computePixel(x, y, pixelData);
        }
    }
}

// Render a portion of the fractal (for multi-threading support)
EMSCRIPTEN_KEEPALIVE
void renderFractalRegion(int startY, int endY) {
    for (int y = startY; y < endY && y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char* pixelData = &outputBuffer[(y * width + x) * 4];
            computePixel(x, y, pixelData);
        }
    }
}

// Get a pointer to the output buffer
EMSCRIPTEN_KEEPALIVE
unsigned char* getOutputBuffer() {
    return outputBuffer;
}

// Move the view by a relative amount
EMSCRIPTEN_KEEPALIVE
void moveView(double deltaX, double deltaY) {
    double scale = 3.0 / (zoom * height);
    centerX += deltaX * scale;
    centerY += deltaY * scale;
}

// Zoom the view
EMSCRIPTEN_KEEPALIVE
void zoomView(double factor, double focusX, double focusY) {
    // Convert screen coordinates to normalized device coordinates
    double ndcX = (focusX / width) * 2.0 - 1.0;
    double ndcY = (focusY / height) * 2.0 - 1.0;
    
    // Scale factors
    double aspectRatio = (double)width / height;
    double scale = 3.0 / (zoom * height);
    
    // Calculate the world position of the focus point
    double worldX = ndcX * scale * aspectRatio * (width / 2.0) + centerX;
    double worldY = ndcY * scale * (height / 2.0) + centerY;
    
    // Adjust zoom
    zoom *= factor;
    
    // Recalculate the center to keep the focus point at the same screen position
    scale = 3.0 / (zoom * height);
    centerX = worldX - ndcX * scale * aspectRatio * (width / 2.0);
    centerY = worldY - ndcY * scale * (height / 2.0);
}

// Accessor functions for the viewport parameters
EMSCRIPTEN_KEEPALIVE
double getCenterX() {
    return centerX;
}

EMSCRIPTEN_KEEPALIVE
double getCenterY() {
    return centerY;
}

EMSCRIPTEN_KEEPALIVE
double getZoom() {
    return zoom;
} 