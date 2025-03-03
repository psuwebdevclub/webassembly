#!/bin/bash

echo "Starting a local server on port 8000..."
echo "Open your browser and navigate to http://localhost:8000"
echo "Press Ctrl+C to stop the server"
echo ""

python3 -m http.server || python -m SimpleHTTPServer 