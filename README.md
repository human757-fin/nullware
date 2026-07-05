# NullWare Browser

A security and developer-friendly web browser with minimal bloatware and built-in toggleable ad-blocker.

## 🚀 Quick Start

### Prerequisites

- **C++17** compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.15+**
- **Git**
- **Python 3.6+**
- **Node.js** (optional, for build tools)

### 1. Clone the repository
```bash
git clone https://github.com/human757-fin/nullware.git
cd nullware
```

### 2. Download CEF (Chromium Embedded Framework)
```bash
# Method 1: Using Python script (recommended)
python tools/clone_deps.py

# Method 2: Manual download
# Download CEF from https://cef-builds.spotifycdn.com/
# Extract to third_party/cef/
```

### 3. Build the browser
```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# On Windows (Visual Studio)
cmake --build . --config Release
```

### 4. Run the browser
```bash
# From build directory
./bin/nullware

# Or from project root
./build/bin/nullware
```

## 🎛️ Command Line Options

| Option | Description |
|--------|-------------|
| `--disable-adblocker` | Disable the built-in ad-blocker |
| `--enable-adblocker` | Enable the built-in ad-blocker |
| `--disable-devtools` | Disable developer tools |
| `--enable-devtools` | Enable developer tools |
| `--disable-sandbox` | Disable sandboxing (not recommended) |
| `--https-only` | Enable HTTPS-only mode |
| `--incognito` | Start in incognito mode |
| `--remote-debugging` | Enable remote debugging |
| `--filter-list=/path/to/file.txt` | Use custom filter list |
| `--user-data-dir=/path/to/dir` | Use custom user data directory |
| `--proxy-server=host:port` | Use HTTP proxy |

### Examples:
```bash
# Disable ad-blocker
./nullware --disable-adblocker

# Enable remote debugging on port 9222
./nullware --remote-debugging

# Use custom user data directory
./nullware --user-data-dir=/tmp/nullware-data

# Enable HTTPS-only mode
./nullware --https-only
```

## ✨ Features

### 🛡️ Security Features
- **Sandboxing**: Process isolation for security
- **Certificate Verification**: SSL/TLS certificate validation
- **Certificate Pinning**: Prevent MITM attacks
- **Content Security Policy**: Enforce CSP headers
- **Mixed Content Blocking**: Prevent HTTP on HTTPS pages
- **HTTPS-Only Mode**: Force encrypted connections
- **Safe Browsing**: Protection against malicious sites

### 🚫 Ad-Blocker
- **EasyList Support**: Compatible with EasyList and EasyPrivacy
- **Toggleable**: Enable/disable via command line or settings
- **Custom Filters**: Add your own filter rules
- **Performance Optimized**: Fast pattern matching
- **Statistics**: Track blocked requests

### 👨‍💻 Developer Tools
- **Web Inspector**: DOM and CSS inspection
- **Network Monitor**: Track network requests
- **JavaScript Console**: Debug and log messages
- **Performance Profiler**: Analyze performance
- **Remote Debugging**: Debug from external tools

### ⚡ Performance
- **Minimal Bloat**: Clean, focused codebase
- **Multi-Process**: Separate processes for stability
- **Hardware Acceleration**: GPU-accelerated rendering
- **Efficient Caching**: Smart resource caching

## 🏗️ Building

### Windows
```bash
# Using Visual Studio
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### macOS
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
```

### Linux
```bash
# Install dependencies
sudo apt-get install -y cmake build-essential libgtk-3-dev libnss3-dev libx11-dev

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

## 📦 CMake Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_TESTS` | Build test suites | ON |
| `ENABLE_ADBLOCKER` | Enable built-in ad-blocker | ON |
| `ENABLE_DEVTOOLS` | Enable developer tools | ON |
| `ENABLE_SANDBOX` | Enable sandboxing | ON |
| `USE_SYSTEM_CEF` | Use system-wide CEF installation | OFF |

Example:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ADBLOCKER=OFF -DENABLE_DEVTOOLS=ON
```

## 🐛 Troubleshooting

### CEF not found
```
Please run: python tools/clone_deps.py
```

### Build errors on Linux
```bash
# Install required dependencies
sudo apt-get install -y cmake build-essential libgtk-3-dev libnss3-dev libx11-dev libxcomposite-dev libxdamage-dev libxi-dev libxrandr-dev libxrender-dev libxtst-dev libasound2-dev libcups2-dev libfreetype6-dev libgbm-dev
```

### Build errors on macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake
```

### Build errors on Windows
- Make sure Visual Studio 2022 is installed
- Install CMake from https://cmake.org/download/
- Make sure Python is in your PATH

## 📚 Documentation

- [Development Guide](docs/DEVELOPMENT.md) - Comprehensive development documentation
- [Architecture](docs/ARCHITECTURE.md) - System architecture overview
- [API Documentation](docs/API.md) - API reference

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests (`ctest`)
5. Commit your changes
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- [Chromium Embedded Framework (CEF)](https://bitbucket.org/chromiumembedded/cef/src/master/)
- [EasyList](https://easylist.to/) - Ad blocking filter lists
- [Chromium Project](https://www.chromium.org/) - The foundation of modern web browsers

---

**NullWare Browser** - Security and developer-friendly web browsing

*Built with ❤️ for privacy and productivity*
