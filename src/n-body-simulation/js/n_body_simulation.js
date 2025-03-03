class NBodySimulation {
    constructor() {
        this.MAX_BODIES = 10000;   // Increased from 5000 to match C implementation
        this.G = 0.1;   // Gravitational constant - increased for visible effect
        this.SOFTENING = 1e-9;  // Prevents division by zero
        
        this.bodies = [];
        this.numBodies = 0;
        this.width = 800;
        this.height = 600;
        this.timeStep = 0.008;  // Time step for simulation
    }
    
    init(canvasWidth, canvasHeight, bodyCount) {
        this.width = canvasWidth;
        this.height = canvasHeight;
        
        this.numBodies = bodyCount > this.MAX_BODIES ? this.MAX_BODIES : bodyCount;
        this.bodies = [];
        
        // Initialize with random values
        for (let i = 0; i < this.numBodies; i++) {
            this.bodies.push({
                // Position: random within canvas
                x: Math.random() * this.width,
                y: Math.random() * this.height,
                
                // Velocity: larger random values for more visible movement
                vx: (Math.random() * 100 - 50) * 0.5,
                vy: (Math.random() * 100 - 50) * 0.5,
                
                // Mass: random between 1 and 100
                mass: 1.0 + Math.random() * 100,
                
                // Radius proportional to mass (for visualization)
                radius: 1.0 + Math.sqrt(1.0 + Math.random() * 100) * 0.5,
                
                // Random color (RGB, fully opaque)
                color: (
                    ((Math.floor(Math.random() * 200) + 55) << 24) |
                    ((Math.floor(Math.random() * 200) + 55) << 16) |
                    ((Math.floor(Math.random() * 200) + 55) << 8) |
                    0xFF
                )
            });
        }
    }
    
    updatePositions() {
        // Calculate forces
        for (let i = 0; i < this.numBodies; i++) {
            let fx = 0, fy = 0;
            
            // Calculate gravitational forces from all other bodies
            for (let j = 0; j < this.numBodies; j++) {
                if (i === j) continue;
                
                // Calculate distance
                const dx = this.bodies[j].x - this.bodies[i].x;
                const dy = this.bodies[j].y - this.bodies[i].y;
                const distSq = dx*dx + dy*dy + this.SOFTENING;
                const dist = Math.sqrt(distSq);
                
                // Newton's law of universal gravitation
                const f = this.G * this.bodies[i].mass * this.bodies[j].mass / distSq;
                
                // Components of the force
                fx += f * dx / dist;
                fy += f * dy / dist;
            }
            
            // Update velocity (F = ma => a = F/m)
            this.bodies[i].vx += fx / this.bodies[i].mass * this.timeStep;
            this.bodies[i].vy += fy / this.bodies[i].mass * this.timeStep;
        }
        
        // Update positions
        for (let i = 0; i < this.numBodies; i++) {
            this.bodies[i].x += this.bodies[i].vx * this.timeStep;
            this.bodies[i].y += this.bodies[i].vy * this.timeStep;
            
            // Simple boundary collision
            if (this.bodies[i].x < 0 || this.bodies[i].x > this.width) {
                this.bodies[i].vx *= -0.9; // Damping
                this.bodies[i].x = this.bodies[i].x < 0 ? 0 : this.width;
            }
            if (this.bodies[i].y < 0 || this.bodies[i].y > this.height) {
                this.bodies[i].vy *= -0.9; // Damping
                this.bodies[i].y = this.bodies[i].y < 0 ? 0 : this.height;
            }
        }
    }
    
    getSimulationData() {
        // Format: [x, y, radius, color, ...] (4 values per body)
        const simData = new Float64Array(this.numBodies * 4);
        
        for (let i = 0; i < this.numBodies; i++) {
            const base = i * 4;
            simData[base + 0] = this.bodies[i].x;
            simData[base + 1] = this.bodies[i].y;
            simData[base + 2] = this.bodies[i].radius;
            simData[base + 3] = this.bodies[i].color;
        }
        
        return simData;
    }
    
    getBodyCount() {
        return this.numBodies;
    }
    
    setTimeStep(ts) {
        this.timeStep = ts;
    }
    
    getTimeStep() {
        return this.timeStep;
    }
}

// Export the NBodySimulation class
if (typeof module !== 'undefined' && module.exports) {
    module.exports = NBodySimulation;
} else {
    window.NBodySimulation = NBodySimulation;
} 