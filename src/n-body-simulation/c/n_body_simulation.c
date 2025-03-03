#include <stdlib.h>
#include <math.h>
#include <emscripten.h>

#define MAX_BODIES 10000   // Increased from 5000 to allow more bodies
#define G 0.1   // Gravitational constant - increased for visible effect
#define SOFTENING 1e-9  // Prevents division by zero

typedef struct {
    double x, y;       // Position
    double vx, vy;     // Velocity
    double mass;       // Mass
    double radius;     // Radius for collision detection
    int color;         // Color as a 32-bit integer (RGBA)
} Body;

// Global bodies array
Body bodies[MAX_BODIES];
int numBodies = 0;
int width = 800;
int height = 600;
double timeStep = 0.008;  // Time step for simulation

// Initialize the simulation
EMSCRIPTEN_KEEPALIVE
void init(int canvasWidth, int canvasHeight, int bodyCount) {
    width = canvasWidth;
    height = canvasHeight;
    
    numBodies = bodyCount > MAX_BODIES ? MAX_BODIES : bodyCount;
    
    // Initialize with random values
    for (int i = 0; i < numBodies; i++) {
        // Position: random within canvas
        bodies[i].x = rand() % width;
        bodies[i].y = rand() % height;
        
        // Velocity: larger random values for more visible movement
        bodies[i].vx = (rand() % 100 - 50) * 0.5;
        bodies[i].vy = (rand() % 100 - 50) * 0.5;
        
        // Mass: random between 1 and 100
        bodies[i].mass = 1.0 + (rand() % 100);
        
        // Radius proportional to mass (for visualization)
        bodies[i].radius = 1.0 + sqrt(bodies[i].mass) * 0.5;
        
        // Random color (RGB, fully opaque)
        bodies[i].color = 
            ((rand() % 200 + 55) << 24) |  // R
            ((rand() % 200 + 55) << 16) |  // G
            ((rand() % 200 + 55) << 8) |   // B
            0xFF;                          // A
    }
}

// Update the simulation for one time step
EMSCRIPTEN_KEEPALIVE
void updatePositions() {
    // Calculate forces
    for (int i = 0; i < numBodies; i++) {
        double fx = 0, fy = 0;
        
        // Calculate gravitational forces from all other bodies
        for (int j = 0; j < numBodies; j++) {
            if (i == j) continue;
            
            // Calculate distance
            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double distSq = dx*dx + dy*dy + SOFTENING;
            double dist = sqrt(distSq);
            
            // Newton's law of universal gravitation
            double f = G * bodies[i].mass * bodies[j].mass / distSq;
            
            // Components of the force
            fx += f * dx / dist;
            fy += f * dy / dist;
        }
        
        // Update velocity (F = ma => a = F/m)
        bodies[i].vx += fx / bodies[i].mass * timeStep;
        bodies[i].vy += fy / bodies[i].mass * timeStep;
    }
    
    // Update positions
    for (int i = 0; i < numBodies; i++) {
        bodies[i].x += bodies[i].vx * timeStep;
        bodies[i].y += bodies[i].vy * timeStep;
        
        // Simple boundary collision
        if (bodies[i].x < 0 || bodies[i].x > width) {
            bodies[i].vx *= -0.9; // Damping
            bodies[i].x = bodies[i].x < 0 ? 0 : width;
        }
        if (bodies[i].y < 0 || bodies[i].y > height) {
            bodies[i].vy *= -0.9; // Damping
            bodies[i].y = bodies[i].y < 0 ? 0 : height;
        }
    }
}

// Get simulation data for rendering
EMSCRIPTEN_KEEPALIVE
double* getSimulationData() {
    // Format: [x, y, radius, color, ...] (4 values per body)
    static double* simData = NULL;
    
    if (simData == NULL) {
        simData = (double*)malloc(MAX_BODIES * 4 * sizeof(double));
    }
    
    for (int i = 0; i < numBodies; i++) {
        int base = i * 4;
        simData[base + 0] = bodies[i].x;
        simData[base + 1] = bodies[i].y;
        simData[base + 2] = bodies[i].radius;
        simData[base + 3] = (double)bodies[i].color;
    }
    
    return simData;
}

// Get number of bodies
EMSCRIPTEN_KEEPALIVE
int getBodyCount() {
    return numBodies;
}

// Set time step for simulation
EMSCRIPTEN_KEEPALIVE
void setTimeStep(double ts) {
    timeStep = ts;
}

// Get current time step
EMSCRIPTEN_KEEPALIVE
double getTimeStep() {
    return timeStep;
} 