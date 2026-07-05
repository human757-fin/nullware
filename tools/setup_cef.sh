#!/bin/bash

# NullWare Browser - CEF Setup Script
# This script helps you download and set up CEF for building NullWare Browser

set -e

echo "NullWare Browser - CEF Setup"
echo "=============================="
echo ""

# Detect platform
PLATFORM=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m | tr '[:upper:]' '[:lower:]')

echo "Detected platform: $PLATFORM"
echo "Detected architecture: $ARCH"
echo ""

# List of CEF versions to try (newest first)
CEF_VERSIONS=(
    "125.7.10+g77bf513+chromium-125.0.6422.60"
    "123.7.1+g8322459+chromium-123.0.6312.105"
    "121.7.1+g9140875+chromium-121.0.6167.139"
    "119.7.1+g884d3e8+chromium-119.0.6045.123"
    "117.7.1+g2434099+chromium-117.0.5938.92"
    "115.7.180+ge06b462+chromium-115.0.5790.170"
    "113.7.1+g9062165+chromium-113.0.5672.98"
    "111.7.1+g4552866+chromium-111.0.5563.64"
    "109.7.1+g5243435+chromium-109.0.5414.119"
    "105.7.14+g475f572+chromium-105.0.5195.127"
)

# Function to try downloading a specific CEF version
try_cef_version() {
    local version=$1
    local platform=$2
    local arch=$3
    
    # Determine file name based on platform
    local cef_file=""
    case "$platform" in
        linux) 
            if [ "$arch" = "x86_64" ] || [ "$arch" = "amd64" ]; then
                cef_file="cef_binary_${version}_linux64_minimal.tar.bz2"
            else
                return 1
            fi
            ;;
        darwin) 
            cef_file="cef_binary_${version}_macosx64_minimal.tar.bz2"
            ;;
        windows|mingw|msys) 
            cef_file="cef_binary_${version}_windows64_minimal.tar.bz2"
            ;;
        *) 
            return 1
            ;;
    esac
    
    local cef_url="https://cef-builds.spotifycdn.com/index/${cef_file}"
    
    echo "Trying CEF version: $version"
    echo "URL: $cef_url"
    
    # Create third_party directory
    mkdir -p third_party
    cd third_party
    
    # Try to download
    if command -v wget &> /dev/null; then
        if wget --spider "$cef_url" 2>/dev/null; then
            echo "Found valid CEF version: $version"
            if [ -f "$cef_file" ]; then
                echo "CEF archive already exists, skipping download"
            else
                echo "Downloading..."
                wget "$cef_url" -O "$cef_file"
            fi
            
            # Extract
            if [ -d "cef" ]; then
                echo "CEF directory already exists, removing..."
                rm -rf cef
            fi
            
            echo "Extracting..."
            tar -xf "$cef_file"
            rm "$cef_file"
            
            echo "CEF $version downloaded and extracted successfully!"
            return 0
        fi
    elif command -v curl &> /dev/null; then
        if curl --output /dev/null --silent --head --fail "$cef_url"; then
            echo "Found valid CEF version: $version"
            if [ -f "$cef_file" ]; then
                echo "CEF archive already exists, skipping download"
            else
                echo "Downloading..."
                curl -L "$cef_url" -o "$cef_file"
            fi
            
            # Extract
            if [ -d "cef" ]; then
                echo "CEF directory already exists, removing..."
                rm -rf cef
            fi
            
            echo "Extracting..."
            tar -xf "$cef_file"
            rm "$cef_file"
            
            echo "CEF $version downloaded and extracted successfully!"
            return 0
        fi
    else
        echo "Neither wget nor curl found, cannot check URLs"
        return 1
    fi
    
    return 1
}

# Try each version until one works
echo "Trying to download CEF..."
echo ""

for version in "${CEF_VERSIONS[@]}"; do
    if try_cef_version "$version" "$PLATFORM" "$ARCH"; then
        echo ""
        echo "CEF is now available in: third_party/cef"
        echo ""
        echo "CEF directory structure:"
        ls -la third_party/cef/ || true
        echo ""
        echo "Setup complete! You can now build NullWare Browser."
        exit 0
    fi
done

echo ""
echo "=========================================="
echo "Failed to download CEF automatically"
echo "=========================================="
echo ""
echo "Please download CEF manually:"
echo ""
echo "1. Visit: https://cef-builds.spotifycdn.com/index/"
echo "2. Find a version for your platform:"
echo "   - Linux: cef_binary_*_linux64_minimal.tar.bz2"
echo "   - macOS: cef_binary_*_macosx64_minimal.tar.bz2"
echo "   - Windows: cef_binary_*_windows64_minimal.tar.bz2"
echo ""
echo "3. Download the minimal version (not the full SDK)"
echo "4. Extract the archive to: third_party/cef/"
echo ""
echo "Example commands:"
echo "  mkdir -p third_party"
echo "  cd third_party"
echo "  wget https://cef-builds.spotifycdn.com/index/cef_binary_115.7.180+ge06b462+chromium-115.0.5790.170_linux64_minimal.tar.bz2"
echo "  tar -xf cef_binary_*_linux64_minimal.tar.bz2"
echo ""
echo "After downloading CEF, you can build with:"
echo "  mkdir build && cd build"
echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "  cmake --build . -j\$(nproc)"
echo ""

exit 1
