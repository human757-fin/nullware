#!/bin/bash

# NullWare Browser - CEF Downloader
# This script downloads CEF for the current platform

set -e

echo "NullWare Browser - CEF Downloader"
echo "=================================="

# Detect platform
PLATFORM=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m | tr '[:upper:]' '[:lower:]')

echo "Detected platform: $PLATFORM"
echo "Detected architecture: $ARCH"

# Use a known working CEF version
# Version 105.7.14 is known to work
CEF_VERSION="105.7.14+g475f572+chromium-105.0.5195.127"

# Determine the CEF file name based on platform
case "$PLATFORM" in
    linux)
        if [ "$ARCH" = "x86_64" ] || [ "$ARCH" = "amd64" ]; then
            CEF_FILE="cef_binary_${CEF_VERSION}_linux64_minimal.tar.bz2"
            CEF_URL="https://cef-builds.spotifycdn.com/index/${CEF_FILE}"
        else
            echo "Unsupported architecture: $ARCH"
            exit 1
        fi
        ;;
    darwin)
        CEF_FILE="cef_binary_${CEF_VERSION}_macosx64_minimal.tar.bz2"
        CEF_URL="https://cef-builds.spotifycdn.com/index/${CEF_FILE}"
        ;;
    windows|mingw|msys)
        CEF_FILE="cef_binary_${CEF_VERSION}_windows64_minimal.tar.bz2"
        CEF_URL="https://cef-builds.spotifycdn.com/index/${CEF_FILE}"
        ;;
    *)
        echo "Unsupported platform: $PLATFORM"
        exit 1
        ;;
esac

echo "CEF URL: $CEF_URL"

# Create third_party directory
mkdir -p third_party
cd third_party

# Download CEF
if [ -f "$CEF_FILE" ]; then
    echo "CEF archive already exists, skipping download"
else
    echo "Downloading CEF..."
    if command -v wget &> /dev/null; then
        wget "$CEF_URL" -O "$CEF_FILE"
    elif command -v curl &> /dev/null; then
        curl -L "$CEF_URL" -o "$CEF_FILE"
    else
        echo "Neither wget nor curl found, cannot download CEF"
        exit 1
    fi
fi

# Extract CEF
if [ -d "cef" ]; then
    echo "CEF directory already exists, removing..."
    rm -rf cef
fi

echo "Extracting CEF..."
tar -xf "$CEF_FILE"

# Clean up
rm "$CEF_FILE"

echo "CEF downloaded and extracted successfully!"
echo "CEF is now available in: third_party/cef"

# Show CEF structure
echo ""
echo "CEF directory structure:"
ls -la cef/ || true
