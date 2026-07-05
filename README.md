# NullWare Browser

A security and developer-friendly web browser with minimal bloatware and built-in toggleable ad-blocker.

## Features

- **Security-First**: Built with privacy and security as core principles
- **Developer-Friendly**: Built-in developer tools and debugging capabilities
- **Minimal Bloat**: Clean, lightweight design without unnecessary features
- **Toggleable Ad-Blocker**: Built-in ad-blocking that can be easily enabled/disabled
- **Cross-Platform**: Support for Windows, macOS, and Linux

## Project Structure

```
nullware/
├── build/                    # Build scripts and configurations
├── docs/                     # Documentation
├── src/
│   ├── browser/             # Core browser functionality
│   ├── common/              # Shared utilities and constants
│   ├── renderer/            # Rendering engine integration
│   ├── security/            # Security modules
│   ├── adblocker/           # Ad-blocking functionality
│   ├── devtools/            # Developer tools
│   ├── ui/                  # User interface components
│   └── main/                # Application entry point
├── third_party/             # Third-party dependencies
├── resources/               # Static resources (icons, etc.)
├── tests/                   # Test suites
├── tools/                   # Development tools and scripts
├── CMakeLists.txt           # Main CMake configuration
├── package.json             # Node.js dependencies for build tools
└── README.md
```

## Getting Started

### Prerequisites

- C++17 compatible compiler
- CMake 3.15+
- Git
- Python 3.6+
- Node.js (for build tools)

### Building

```bash
# Clone dependencies
./tools/clone_deps.py

# Configure build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the browser
cmake --build . -j$(nproc)

# Run the browser
./nullware
```

### Development

```bash
# Run tests
ctest --output-on-failure

# Build in debug mode
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Configuration

Edit `src/common/config.h` to customize browser behavior and features.

## Security Features

- Sandboxed rendering processes
- Certificate pinning
- Content Security Policy (CSP) enforcement
- Secure cookie handling
- Built-in HTTPS-only mode

## Ad-Blocker

The ad-blocker uses EasyList and EasyPrivacy filters by default. It can be toggled on/off via the settings menu or command line:

```bash
# Disable ad-blocker
./nullware --disable-adblocker

# Use custom filter lists
./nullware --filter-list=/path/to/custom.txt
```

## Developer Tools

- Built-in Web Inspector
- Network traffic monitoring
- Performance profiling
- Console logging
- Element inspection

## License

MIT License - see LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests
5. Submit a pull request

## Architecture Overview

The browser is built on top of the Chromium Embedded Framework (CEF) with additional security layers and developer tools. The architecture follows a multi-process model with clear separation between browser, renderer, and GPU processes.

### Process Model

- **Browser Process**: Manages the browser UI, disk/network access
- **Renderer Processes**: Handle page rendering and JavaScript execution
- **GPU Process**: Handles GPU-accelerated rendering
- **Utility Processes**: Handle specific background tasks

### Security Boundaries

- Sandboxing between processes
- Privilege separation
- Memory isolation
- Network request validation
