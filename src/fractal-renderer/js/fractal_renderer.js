class FractalRenderer {
    constructor() {
        // Constants
        this.MAX_ITERATIONS = 1000;
        this.PALETTE_SIZE = 256;
        
        // Canvas dimensions
        this.width = 0;
        this.height = 0;
        
        // Viewport parameters
        this.centerX = -0.7;
        this.centerY = 0.0;
        this.zoom = 1.0;
        
        // Color palette (RGB)
        this.palette = new Uint8Array(this.PALETTE_SIZE * 3);
        
        // Output buffer (RGBA)
        this.outputBuffer = null;
    }
    
    // Initialize the fractal renderer
    init(canvasWidth, canvasHeight) {
        this.width = canvasWidth;
        this.height = canvasHeight;
        
        // Allocate buffer for output (RGBA format)
        this.outputBuffer = new Uint8ClampedArray(this.width * this.height * 4);
        
        // Initialize color palette (smooth gradient)
        for (let i = 0; i < this.PALETTE_SIZE; i++) {
            // Create a visually pleasing color palette
            const t = i / this.PALETTE_SIZE;
            
            // Use a combination of sine waves for a smooth colorful gradient
            const r = Math.sin(t * 5.0) * 0.5 + 0.5;
            const g = Math.sin(t * 3.0 + 2.0) * 0.5 + 0.5;
            const b = Math.sin(t * 9.0 + 1.0) * 0.5 + 0.5;
            
            this.palette[i * 3 + 0] = Math.floor(r * 255);
            this.palette[i * 3 + 1] = Math.floor(g * 255);
            this.palette[i * 3 + 2] = Math.floor(b * 255);
        }
    }
    
    // Set the view parameters
    setView(x, y, zoomFactor) {
        this.centerX = x;
        this.centerY = y;
        this.zoom = zoomFactor;
    }
    
    // Map iteration count to a smooth color value
    smoothColor(iterations, x, y) {
        if (iterations === this.MAX_ITERATIONS) {
            return 0.0;
        }
        
        // Smooth coloring algorithm using the log
        const zn = Math.sqrt(x*x + y*y);
        const nu = Math.log(Math.log(zn) / Math.log(2.0)) / Math.log(2.0);
        return iterations + 1 - nu;
    }
    
    // Compute Mandelbrot set for a given pixel
    computePixel(px, py, output, offset) {
        // Map pixel coordinates to complex plane
        const aspectRatio = this.width / this.height;
        const scale = 3.0 / (this.zoom * this.height);
        
        const x0 = (px - this.width/2) * scale * aspectRatio + this.centerX;
        const y0 = (py - this.height/2) * scale + this.centerY;
        
        // Mandelbrot iteration variables
        let x = 0.0;
        let y = 0.0;
        let x2 = 0.0;
        let y2 = 0.0;
        let iteration = 0;
        
        // Cardioid and period-2 bulb check (optimization)
        const q = (x0 - 0.25) * (x0 - 0.25) + y0 * y0;
        if (q * (q + (x0 - 0.25)) < 0.25 * y0 * y0 || 
            (x0 + 1.0) * (x0 + 1.0) + y0 * y0 < 0.0625) {
            // Inside the main cardioid or period-2 bulb
            iteration = this.MAX_ITERATIONS;
        } else {
            // Iterate the Mandelbrot formula: z = z^2 + c
            while (x2 + y2 < 4.0 && iteration < this.MAX_ITERATIONS) {
                y = 2 * x * y + y0;
                x = x2 - y2 + x0;
                x2 = x * x;
                y2 = y * y;
                iteration++;
            }
        }
        
        // Apply color mapping
        if (iteration === this.MAX_ITERATIONS) {
            // Inside the set - black
            output[offset + 0] = 0;
            output[offset + 1] = 0;
            output[offset + 2] = 0;
            output[offset + 3] = 255; // Alpha
        } else {
            // Calculate smooth color value
            const colorValue = this.smoothColor(iteration, x, y);
            
            // Map to palette
            const colorIndex = Math.floor(colorValue * 10.0) % this.PALETTE_SIZE;
            output[offset + 0] = this.palette[colorIndex * 3 + 0];
            output[offset + 1] = this.palette[colorIndex * 3 + 1];
            output[offset + 2] = this.palette[colorIndex * 3 + 2];
            output[offset + 3] = 255; // Alpha
        }
    }
    
    // Render the entire fractal
    renderFractal() {
        // Compute each pixel
        for (let y = 0; y < this.height; y++) {
            for (let x = 0; x < this.width; x++) {
                const offset = (y * this.width + x) * 4;
                this.computePixel(x, y, this.outputBuffer, offset);
            }
        }
    }
    
    // Render a portion of the fractal (for multi-threading compatibility)
    renderFractalRegion(startY, endY) {
        for (let y = startY; y < endY && y < this.height; y++) {
            for (let x = 0; x < this.width; x++) {
                const offset = (y * this.width + x) * 4;
                this.computePixel(x, y, this.outputBuffer, offset);
            }
        }
    }
    
    // Get the output buffer
    getOutputBuffer() {
        return this.outputBuffer;
    }
    
    // Move the view by a relative amount
    moveView(deltaX, deltaY) {
        const scale = 3.0 / (this.zoom * this.height);
        this.centerX += deltaX * scale;
        this.centerY += deltaY * scale;
    }
    
    // Zoom the view
    zoomView(factor, focusX, focusY) {
        // Convert screen coordinates to normalized device coordinates
        const ndcX = (focusX / this.width) * 2.0 - 1.0;
        const ndcY = (focusY / this.height) * 2.0 - 1.0;
        
        // Scale factors
        const aspectRatio = this.width / this.height;
        let scale = 3.0 / (this.zoom * this.height);
        
        // Calculate the world position of the focus point
        const worldX = ndcX * scale * aspectRatio * (this.width / 2.0) + this.centerX;
        const worldY = ndcY * scale * (this.height / 2.0) + this.centerY;
        
        // Adjust zoom
        this.zoom *= factor;
        
        // Recalculate the center to keep the focus point at the same screen position
        scale = 3.0 / (this.zoom * this.height);
        this.centerX = worldX - ndcX * scale * aspectRatio * (this.width / 2.0);
        this.centerY = worldY - ndcY * scale * (this.height / 2.0);
    }
}

// Export the FractalRenderer class
export default FractalRenderer; 