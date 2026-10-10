#!/bin/bash

# File URL and local name
URL="https://github.com/magafurovamir-tech/pack/releases/download/v1.0.0/pack-linux"
OUTPUT="pack-linux"

echo "Downloading $OUTPUT..."
if curl -L -o "$OUTPUT" "$URL"; then
    echo "Successfully downloaded. Making the file executable..."
    chmod +x "$OUTPUT"
    
    echo "Running $OUTPUT..."
    ./"$OUTPUT"
else
    echo "Error: Failed to download the file."
    exit 1
fi
