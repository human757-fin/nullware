#!/usr/bin/env python3
"""
Clone dependencies for NullWare Browser
This script downloads and sets up the required dependencies for building the browser.
"""

import os
import sys
import platform
import subprocess
import shutil
import urllib.request
import tarfile
import zipfile
import json
import hashlib
from pathlib import Path

# Configuration
DEPS_DIR = "third_party"
CEF_VERSION = "115.7.180+ge06b462+chromium-115.0.5790.170"
CEF_DOWNLOAD_URL = f"https://cef-builds.spotifycdn.com/index/cef_binary_{CEF_VERSION}_windows64_minimal.tar.bz2"

# Platform-specific configurations
PLATFORM_CONFIGS = {
    "windows": {
        "cef_url": f"https://cef-builds.spotifycdn.com/index/cef_binary_{CEF_VERSION}_windows64_minimal.tar.bz2",
        "cef_dir": "cef",
        "dependencies": []
    },
    "macos": {
        "cef_url": f"https://cef-builds.spotifycdn.com/index/cef_binary_{CEF_VERSION}_macosx64_minimal.tar.bz2",
        "cef_dir": "cef",
        "dependencies": []
    },
    "linux": {
        "cef_url": f"https://cef-builds.spotifycdn.com/index/cef_binary_{CEF_VERSION}_linux64_minimal.tar.bz2",
        "cef_dir": "cef",
        "dependencies": [
            "libgtk-3-dev",
            "libnss3-dev",
            "libx11-dev",
            "libxcomposite-dev",
            "libxdamage-dev",
            "libxi-dev",
            "libxrandr-dev",
            "libxrender-dev",
            "libxtst-dev",
            "libasound2-dev",
            "libcups2-dev",
            "libfreetype6-dev",
            "libgbm-dev"
        ]
    }
}

def get_platform():
    """Get the current platform."""
    system = platform.system().lower()
    if system == "windows":
        return "windows"
    elif system == "darwin":
        return "macos"
    elif system == "linux":
        return "linux"
    else:
        print(f"Unsupported platform: {system}")
        sys.exit(1)

def download_file(url, destination):
    """Download a file from a URL."""
    print(f"Downloading {url}...")
    
    try:
        # Create parent directory if it doesn't exist
        os.makedirs(os.path.dirname(destination), exist_ok=True)
        
        # Download the file
        urllib.request.urlretrieve(url, destination)
        print(f"Downloaded to {destination}")
        return True
    except Exception as e:
        print(f"Failed to download {url}: {e}")
        return False

def extract_tar_bz2(file_path, extract_to):
    """Extract a .tar.bz2 file."""
    print(f"Extracting {file_path} to {extract_to}...")
    
    try:
        with tarfile.open(file_path, "r:bz2") as tar:
            tar.extractall(path=extract_to)
        print("Extraction complete")
        return True
    except Exception as e:
        print(f"Failed to extract {file_path}: {e}")
        return False

def extract_zip(file_path, extract_to):
    """Extract a .zip file."""
    print(f"Extracting {file_path} to {extract_to}...")
    
    try:
        with zipfile.ZipFile(file_path, 'r') as zip_ref:
            zip_ref.extractall(extract_to)
        print("Extraction complete")
        return True
    except Exception as e:
        print(f"Failed to extract {file_path}: {e}")
        return False

def install_system_dependencies(platform_name):
    """Install system dependencies for the current platform."""
    config = PLATFORM_CONFIGS.get(platform_name, {})
    dependencies = config.get("dependencies", [])
    
    if not dependencies:
        print("No system dependencies to install")
        return True
    
    print(f"Installing system dependencies: {', '.join(dependencies)}")
    
    try:
        if platform_name == "linux":
            # Use apt-get on Debian/Ubuntu
            subprocess.run(["sudo", "apt-get", "update"], check=True)
            subprocess.run(["sudo", "apt-get", "install", "-y"] + dependencies, check=True)
        elif platform_name == "macos":
            # Use brew on macOS
            subprocess.run(["brew", "install"] + dependencies, check=True)
        elif platform_name == "windows":
            # Windows dependencies would be installed differently
            print("Please install Windows dependencies manually")
            return False
        
        print("System dependencies installed successfully")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Failed to install system dependencies: {e}")
        return False

def clone_cef(platform_name):
    """Clone CEF (Chromium Embedded Framework) for the current platform."""
    config = PLATFORM_CONFIGS.get(platform_name, {})
    cef_url = config.get("cef_url", "")
    cef_dir = config.get("cef_dir", "cef")
    
    if not cef_url:
        print("No CEF URL configured for this platform")
        return False
    
    # Create third_party directory if it doesn't exist
    os.makedirs(DEPS_DIR, exist_ok=True)
    
    # Download CEF
    cef_archive = os.path.join(DEPS_DIR, os.path.basename(cef_url))
    if not download_file(cef_url, cef_archive):
        return False
    
    # Extract CEF
    cef_path = os.path.join(DEPS_DIR, cef_dir)
    if os.path.exists(cef_path):
        print(f"Removing existing CEF directory: {cef_path}")
        shutil.rmtree(cef_path)
    
    if cef_archive.endswith(".tar.bz2"):
        if not extract_tar_bz2(cef_archive, DEPS_DIR):
            return False
    elif cef_archive.endswith(".zip"):
        if not extract_zip(cef_archive, DEPS_DIR):
            return False
    else:
        print(f"Unknown archive format: {cef_archive}")
        return False
    
    # Clean up the archive
    os.remove(cef_archive)
    
    print(f"CEF cloned successfully to {cef_path}")
    return True

def clone_dependencies():
    """Clone all dependencies for the current platform."""
    platform_name = get_platform()
    print(f"Cloning dependencies for {platform_name}...")
    
    # Install system dependencies first
    if not install_system_dependencies(platform_name):
        print("Warning: Some system dependencies may not be installed")
    
    # Clone CEF
    if not clone_cef(platform_name):
        print("Warning: Failed to clone CEF")
        return False
    
    print("Dependencies cloned successfully")
    return True

def main():
    """Main function."""
    print("NullWare Browser - Dependency Cloner")
    print("=" * 40)
    
    if not clone_dependencies():
        print("Failed to clone dependencies")
        sys.exit(1)
    
    print("\nDependency cloning complete!")
    print("You can now build the browser using:")
    print("  mkdir build && cd build")
    print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("  cmake --build . -j$(nproc)")

if __name__ == "__main__":
    main()
