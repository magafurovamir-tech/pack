#!/bin/bash

# File URL and local name
URL="https://github.com/magafurovamir-tech/pack/releases/download/v1.0.0/pack-linux"
OUTPUT="pack-linux"
INSTALL_DIR="/usr/bin"

echo "Downloading $OUTPUT..."
if curl -L -o "$OUTPUT" "$URL"; then
    echo "Successfully downloaded. Making the file executable..."
    chmod +x "$OUTPUT"
    
    echo "Moving $OUTPUT to $INSTALL_DIR (requires sudo)..."
    sudo mv "$OUTPUT" "$INSTALL_DIR/$OUTPUT"
    
    echo "Installation complete! You can now run '$OUTPUT' from anywhere."
else
    echo "Error: Failed to download the file."
    exit 1
fi
