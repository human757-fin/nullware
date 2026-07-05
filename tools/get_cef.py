#!/usr/bin/env python3
"""
NullWare Browser - CEF Downloader
This script helps you download the correct CEF version for your platform.
"""

import os
import sys
import platform
import subprocess
import shutil
import urllib.request
import tarfile
import re

def get_platform():
    """Get the current platform."""
    system = platform.system().lower()
    machine = platform.machine().lower()
    
    if system == "windows":
        return "windows", "64" if "64" in machine else "32"
    elif system == "darwin":
        return "macos", "64"
    elif system == "linux":
        return "linux", "64" if "64" in machine else "32"
    else:
        print(f"Unsupported platform: {system}")
        return None, None

def main():
    print("NullWare Browser - CEF Downloader")
    print("=" * 40)
    
    platform_name, arch = get_platform()
    if not platform_name:
        sys.exit(1)
    
    print(f"Detected platform: {platform_name}")
    print(f"Detected architecture: {arch}-bit")
    print()
    
    # Known working CEF versions (as of July 2026)
    # These versions are available on the CEF build server
    cef_versions = [
        # Newer versions
        "125.7.10",
        "123.7.1", 
        "121.7.1",
        "119.7.1",
        "117.7.1",
        # Well-tested versions
        "115.7.180",
        "113.7.1",
        "111.7.1",
        "109.7.1",
        "105.7.14",
    ]
    
    # Platform-specific file patterns
    if platform_name == "linux":
        file_pattern = "cef_binary_{version}+*+chromium-*_linux{arch}_minimal.tar.bz2"
        url_template = "https://cef-builds.spotifycdn.com/{file}"
    elif platform_name == "macos":
        file_pattern = "cef_binary_{version}+*+chromium-*_macosx{arch}_minimal.tar.bz2"
        url_template = "https://cef-builds.spotifycdn.com/{file}"
    elif platform_name == "windows":
        file_pattern = "cef_binary_{version}+*+chromium-*_windows{arch}_minimal.tar.bz2"
        url_template = "https://cef-builds.spotifycdn.com/{file}"
    
    print("Trying to find available CEF versions...")
    print()
    
    # Try to list available files from the server
    try:
        import requests
        response = requests.get("https://cef-builds.spotifycdn.com/index/")
        if response.status_code == 200:
            # Parse HTML to find available files
            available_files = re.findall(r'cef_binary_([^"<>]+)_' + platform_name + r'64_minimal', response.text)
            if available_files:
                print("Available CEF versions for your platform:")
                for i, version in enumerate(available_files[:10]):  # Show first 10
                    print(f"  {i+1}. {version}")
                print()
                
                # Use the first available version
                latest_version = available_files[0]
                print(f"Using latest version: {latest_version}")
                return try_download(latest_version, platform_name, arch, url_template)
    except ImportError:
        print("requests module not available")
    except Exception as e:
        print(f"Could not fetch from server: {e}")
    
    # Fallback: try known versions
    print("Trying known CEF versions...")
    print()
    
    for version in cef_versions:
        if try_download(version, platform_name, arch, url_template):
            return True
    
    # If all else fails, provide manual instructions
    print()
    print("=" * 60)
    print("AUTOMATIC DOWNLOAD FAILED")
    print("=" * 60)
    print()
    print("Please download CEF manually:")
    print()
    print("1. Visit: https://cef-builds.spotifycdn.com/index/")
    print()
    print("2. Find a file matching your platform:")
    if platform_name == "linux":
        print("   - Look for: cef_binary_*_linux64_minimal.tar.bz2")
    elif platform_name == "macos":
        print("   - Look for: cef_binary_*_macosx64_minimal.tar.bz2")
    elif platform_name == "windows":
        print("   - Look for: cef_binary_*_windows64_minimal.tar.bz2")
    print()
    print("3. Download the file")
    print()
    print("4. Extract to: third_party/cef/")
    print()
    print("Example commands:")
    print("  mkdir -p third_party")
    print("  cd third_party")
    print("  tar -xf cef_binary_*_linux64_minimal.tar.bz2")
    print()
    
    return False

def try_download(version, platform_name, arch, url_template):
    """Try to download a specific CEF version."""
    import glob
    
    # Try to find the exact file on the server
    try:
        import requests
        response = requests.get("https://cef-builds.spotifycdn.com/index/")
        if response.status_code == 200:
            # Look for files matching this version
            pattern = f'cef_binary_{version}[^"<>]*_{platform_name}{arch}_minimal'
            matches = re.findall(pattern, response.text)
            if matches:
                file_name = matches[0] + ".tar.bz2"
                url = f"https://cef-builds.spotifycdn.com/{file_name}"
                print(f"Found: {file_name}")
                return download_and_extract(url, file_name)
    except:
        pass
    
    # Try direct URL patterns
    url_patterns = [
        f"cef_binary_{version}+g*+chromium-*_{platform_name}{arch}_minimal.tar.bz2",
        f"cef_binary_{version}_{platform_name}{arch}_minimal.tar.bz2",
    ]
    
    for pattern in url_patterns:
        try:
            import requests
            response = requests.get("https://cef-builds.spotifycdn.com/index/")
            if response.status_code == 200:
                matches = re.findall(pattern.replace("*", "[^\"<>]*"), response.text)
                if matches:
                    file_name = matches[0]
                    url = f"https://cef-builds.spotifycdn.com/{file_name}"
                    print(f"Trying: {file_name}")
                    return download_and_extract(url, file_name)
        except:
            continue
    
    return False

def download_and_extract(url, file_name):
    """Download and extract CEF."""
    print(f"Downloading: {url}")
    
    try:
        # Create third_party directory
        os.makedirs("third_party", exist_ok=True)
        
        # Download
        file_path = os.path.join("third_party", file_name)
        urllib.request.urlretrieve(url, file_path)
        print(f"Downloaded: {file_path}")
        
        # Extract
        cef_dir = os.path.join("third_party", "cef")
        if os.path.exists(cef_dir):
            print(f"Removing existing CEF directory: {cef_dir}")
            shutil.rmtree(cef_dir)
        
        print(f"Extracting to: {cef_dir}")
        with tarfile.open(file_path, "r:bz2") as tar:
            tar.extractall(path="third_party")
        
        # Clean up
        os.remove(file_path)
        print(f"CEF extracted successfully to: {cef_dir}")
        
        # Verify
        if os.path.exists(os.path.join(cef_dir, "include", "cef_base.h")):
            print("✅ CEF is ready to use!")
            return True
        else:
            print("❌ CEF extraction failed - cef_base.h not found")
            return False
            
    except Exception as e:
        print(f"❌ Failed: {e}")
        if os.path.exists(file_path):
            os.remove(file_path)
        return False

if __name__ == "__main__":
    success = main()
    if not success:
        print()
        print("Please check CEF_DOWNLOAD_INSTRUCTIONS.md for manual download steps")
        sys.exit(1)
    else:
        print()
        print("You can now build NullWare Browser:")
        print("  mkdir build && cd build")
        print("  cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("  cmake --build . -j$(nproc)")
