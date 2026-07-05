# NullWare Browser - Development Guide

This document provides comprehensive information for developing and contributing to NullWare Browser.

## Table of Contents

1. [Project Structure](#project-structure)
2. [Building the Browser](#building-the-browser)
3. [Architecture Overview](#architecture-overview)
4. [Core Components](#core-components)
5. [Ad-Blocker Implementation](#ad-blocker-implementation)
6. [Security Features](#security-features)
7. [Developer Tools](#developer-tools)
8. [Contributing Guidelines](#contributing-guidelines)
9. [Debugging](#debugging)
10. [Testing](#testing)

## Project Structure

```
nullware/
├── build/                    # Build output directory
├── docs/                     # Documentation
│   ├── DEVELOPMENT.md       # This file
│   ├── ARCHITECTURE.md       # Architecture documentation
│   └── API.md               # API documentation
├── src/
│   ├── common/              # Shared utilities and base classes
│   │   ├── config.h/cpp      # Configuration management
│   │   ├── logging.h/cpp     # Logging system
│   │   ├── string_utils.h/cpp # String utilities
│   │   └── platform_utils.h/cpp # Platform-specific utilities
│   │
│   ├── browser/             # Core browser functionality
│   │   ├── main.cpp          # Application entry point
│   │   ├── browser_main.h/cpp # Main browser application
│   │   ├── browser_window.h/cpp # Browser window management
│   │   ├── browser_client.h/cpp # CEF client implementation
│   │   ├── render_handler.h/cpp # Render handler
│   │   ├── load_handler.h/cpp # Page load handler
│   │   └── navigation_controller.h/cpp # Navigation control
│   │
│   ├── renderer/            # Rendering engine integration
│   │   ├── render_process_handler.h/cpp # Render process handler
│   │   └── render_delegate.h/cpp # Render delegate
│   │
│   ├── security/            # Security modules
│   │   ├── security_manager.h/cpp # Main security manager
│   │   ├── certificate_verifier.h/cpp # Certificate verification
│   │   ├── sandbox_policy.h/cpp # Sandbox policy
│   │   └── content_security_policy.h/cpp # CSP implementation
│   │
│   ├── adblocker/           # Ad-blocking functionality
│   │   ├── ad_blocker.h/cpp # Main ad blocker
│   │   ├── filter_engine.h/cpp # Filter rule engine
│   │   ├── filter_list_loader.h/cpp # Filter list loading
│   │   ├── request_interceptor.h/cpp # Request interception
│   │   └── easylist_parser.h/cpp # EasyList format parser
│   │
│   ├── devtools/            # Developer tools
│   │   ├── devtools_manager.h/cpp # DevTools manager
│   │   ├── inspector.h/cpp   # Web inspector
│   │   ├── network_monitor.h/cpp # Network traffic monitor
│   │   ├── console.h/cpp     # JavaScript console
│   │   └── profiler.h/cpp    # Performance profiler
│   │
│   └── ui/                  # User interface components
│       ├── main_window.h/cpp # Main application window
│       ├── toolbar.h/cpp     # Browser toolbar
│       ├── address_bar.h/cpp # URL address bar
│       ├── tab_strip.h/cpp   # Tab management
│       ├── settings_dialog.h/cpp # Settings dialog
│       └── about_dialog.h/cpp # About dialog
│
├── third_party/             # Third-party dependencies
│   └── cef/                 # Chromium Embedded Framework
├── resources/               # Static resources
│   ├── windows/             # Windows-specific resources
│   │   └── resource.rc      # Windows resource file
│   ├── macos/               # macOS-specific resources
│   │   └── Info.plist       # macOS info plist
│   └── linux/               # Linux-specific resources
├── tests/                   # Test suites
│   ├── unit/                # Unit tests
│   └── integration/         # Integration tests
├── tools/                   # Development tools and scripts
│   ├── clone_deps.py        # Dependency cloning script
│   ├── build.js             # Build automation script
│   ├── clean.js             # Clean build script
│   ├── test.js              # Test runner
│   └── package.js           # Packaging script
├── .gitignore               # Git ignore patterns
├── CMakeLists.txt           # Main CMake configuration
├── package.json             # Node.js dependencies
├── README.md                # Project readme
└── LICENSE                  # License file
```

## Building the Browser

### Prerequisites

#### Windows
- Visual Studio 2022 (Community Edition or higher)
- CMake 3.15 or higher
- Git
- Python 3.6 or higher
- Node.js (for build tools)

#### macOS
- Xcode 12 or higher
- CMake 3.15 or higher
- Git
- Python 3.6 or higher
- Node.js (for build tools)
- Homebrew (for dependencies)

#### Linux
- GCC 9 or higher / Clang 10 or higher
- CMake 3.15 or higher
- Git
- Python 3.6 or higher
- Node.js (for build tools)
- Required libraries: GTK3, NSS, X11, etc.

### Building

#### 1. Clone the repository
```bash
git clone https://github.com/human757-fin/nullware.git
cd nullware
```

#### 2. Clone dependencies
```bash
# Using Python script
python tools/clone_deps.py

# Or using Node.js
npm run download-deps
```

#### 3. Configure the build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### 4. Build the browser
```bash
# On Windows (Visual Studio)
cmake --build . --config Release

# On Unix-like systems
cmake --build . -j$(nproc)
```

#### 5. Run the browser
```bash
# From the build directory
./bin/nullware

# Or from the project root
./build/bin/nullware
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_TESTS` | Build test suites | ON |
| `ENABLE_ADBLOCKER` | Enable built-in ad-blocker | ON |
| `ENABLE_DEVTOOLS` | Enable developer tools | ON |
| `ENABLE_SANDBOX` | Enable sandboxing | ON |
| `USE_SYSTEM_CEF` | Use system-wide CEF installation | OFF |

Example with custom options:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ADBLOCKER=OFF -DENABLE_DEVTOOLS=ON
```

## Architecture Overview

NullWare Browser is built on top of the Chromium Embedded Framework (CEF) with additional security layers and developer tools. The architecture follows a multi-process model for security and stability.

### Process Model

1. **Browser Process**: Manages the browser UI, disk/network access, and coordinates between renderer processes.
2. **Renderer Processes**: Handle page rendering and JavaScript execution in isolated processes.
3. **GPU Process**: Handles GPU-accelerated rendering.
4. **Utility Processes**: Handle specific background tasks.

### Security Boundaries

- **Sandboxing**: Each process runs in a sandbox with restricted permissions.
- **Privilege Separation**: Browser process has more privileges than renderer processes.
- **Memory Isolation**: Processes are isolated from each other to prevent memory corruption.
- **Network Request Validation**: All network requests are validated before processing.

### Component Interaction

```
┌─────────────────────────────────────────────────────────────┐
│                      Browser Process                            │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐ │
│  │   Browser   │  │  Security    │  │      Ad-Blocker          │ │
│  │   Main      │  │  Manager     │  │                           │ │
│  └──────┬──────┘  └──────┬──────┘  └──────────┬────────────┘ │
│         │                │                   │                │
│         ▼                ▼                   ▼                │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                    CEF Framework                           │ │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │ │
│  │  │   Browser   │  │   Renderer   │  │      Network         │ │ │
│  │  │   Client    │  │   Client     │  │      Service        │ │ │
│  │  └──────┬──────┘  └──────┬──────┘  └──────────┬───────────┘ │ │
│  └─────────┼────────────────┼──────────────────┼─────────────┘ │
└───────────┼────────────────┼──────────────────┼───────────────┘
            │                │                   │
            ▼                ▼                   ▼
┌─────────────────┐ ┌─────────────┐ ┌─────────────────────┐
│   Renderer      │ │   Renderer   │ │      Network        │
│   Process 1     │ │   Process 2  │ │      Stack          │
│                 │ │              │ │                      │
└─────────────────┘ └─────────────┘ └─────────────────────┘
```

## Core Components

### Configuration Management

The `ConfigManager` class handles all browser configuration, including:
- Ad-blocker settings
- Security settings
- Developer tools settings
- Path configurations
- Proxy settings
- Privacy settings

Configuration can be loaded from:
- Command line arguments
- Configuration files
- Environment variables

### Logging System

The `Logger` class provides a flexible logging system with:
- Multiple log levels (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
- Multiple output targets (CONSOLE, FILE, SYSLOG, ALL)
- Thread-safe logging
- Timestamp and thread ID in log messages

Example usage:
```cpp
#include "src/common/logging.h"

LOG_INFO("Browser starting...");
LOG_ERROR("Failed to load configuration: " + error_message);

// Stream-based logging
ILOG << "Loading page: " << url << " from cache: " << (from_cache ? "yes" : "no");
```

### String Utilities

The `StringUtils` namespace provides comprehensive string manipulation functions:
- Trimming, case conversion
- URL encoding/decoding
- Base64 encoding/decoding
- Hex encoding/decoding
- String formatting
- URL parsing
- Pattern matching
- Hashing (MD5, SHA1, SHA256)

## Ad-Blocker Implementation

### Overview

The ad-blocker is built as a modular system with the following components:

1. **AdBlocker**: Main interface for ad-blocking functionality
2. **FilterEngine**: Manages and matches filter rules
3. **RequestInterceptor**: Intercepts and filters network requests
4. **FilterListLoader**: Loads filter lists from various sources
5. **EasyListParser**: Parses EasyList format filter rules

### Filter Rule Types

- **Blocking Rules**: Block requests matching specific patterns
- **Exception Rules**: Allow requests that would otherwise be blocked
- **Element Hiding Rules**: Hide DOM elements matching CSS selectors
- **CSS Rules**: Inject CSS styles
- **Script Injection Rules**: Inject JavaScript

### Filter Rule Syntax

NullWare supports EasyList and EasyPrivacy filter formats:

```
# Basic blocking rule
||example.com/ads/*

# Exception rule (allow)
@@||example.com/ads/important.js

# Domain-specific rule
example.com##.ad-banner

# Third-party only
||example.com/*$third-party

# Specific resource types
||example.com/*$script,image

# Match case
||Example.com/*$match-case

# Important (override other rules)
||example.com/*$important
```

### Integration with CEF

The ad-blocker integrates with CEF through the `RequestInterceptor` class, which implements CEF's `CefResourceRequestHandler` interface to intercept and filter network requests.

### Performance Optimization

- **Compiled Regex**: Filter rules are compiled to regex patterns for faster matching
- **Domain Indexing**: Rules are indexed by domain for faster lookup
- **Caching**: Filter lists are cached locally to avoid repeated downloads
- **Lazy Loading**: Filter lists are loaded on-demand

### Custom Filter Lists

Users can add custom filter lists:

```cpp
// Add a custom filter list from URL
AdBlockerManager::GetInstance().GetAdBlocker()->LoadFilterListFromUrl(
    "https://example.com/custom-filters.txt"
);

// Add a custom filter list from file
AdBlockerManager::GetInstance().GetAdBlocker()->LoadFilterListFromFile(
    "/path/to/custom-filters.txt"
);

// Add individual filter rules
AdBlockerManager::GetInstance().GetAdBlocker()->AddCustomFilter(
    "||malicious-site.com/*"
);
```

## Security Features

### Security Manager

The `SecurityManager` class coordinates all security-related functionality:

- **URL Safety Checking**: Checks URLs against known malicious sites
- **Certificate Verification**: Validates SSL/TLS certificates
- **Content Security Policy**: Enforces CSP headers
- **Mixed Content Blocking**: Prevents HTTP resources on HTTPS pages
- **Script Execution Control**: Controls JavaScript execution

### Certificate Verification

The `CertificateVerifier` class handles:
- Certificate chain validation
- Expiration date checking
- Revocation list checking (OCSP/CRL)
- Certificate pinning
- Self-signed certificate handling

### Sandbox Policy

The `SandboxPolicy` class manages process sandboxing:
- **Browser Process**: Full access to system resources
- **Renderer Processes**: Restricted access, no direct file system access
- **GPU Process**: Limited access for graphics operations
- **Utility Processes**: Custom sandbox policies based on function

### Content Security Policy

The `ContentSecurityPolicy` class enforces CSP headers:
- Restricts sources for scripts, styles, images, etc.
- Prevents inline script execution
- Blocks eval() and similar functions
- Enforces frame ancestors
- Reports CSP violations

### Security Levels

| Level | Description |
|-------|-------------|
| LOW | Minimal security, allows most content |
| MEDIUM | Balanced security, blocks known threats |
| HIGH | Strict security, blocks potentially dangerous content |
| CUSTOM | User-defined security settings |

## Developer Tools

### Overview

NullWare includes comprehensive developer tools for web development and debugging:

1. **Inspector**: DOM and CSS inspection
2. **Network Monitor**: Network request monitoring and analysis
3. **Console**: JavaScript console with logging
4. **Profiler**: Performance profiling and analysis
5. **Memory Tools**: Memory usage analysis
6. **Storage Tools**: Local storage, cookies, and cache inspection

### DevTools Manager

The `DevToolsManager` class manages all developer tools:

```cpp
// Enable developer tools
DevToolsManager::GetInstance().SetEnabled(true);

// Open DevTools for a page
DevToolsManager::GetInstance().OpenDevTools(page_id, DevToolsType::INSPECTOR);

// Log to console
DevToolsManager::GetInstance().LogToConsole(page_id, "Hello, world!", "log");

// Record network request
NetworkRequestInfo request;
request.url = "https://example.com/api";
request.method = "GET";
DevToolsManager::GetInstance().RecordNetworkRequest(page_id, request);

// Start profiling
DevToolsManager::GetInstance().StartProfiling(page_id, "Performance Test");

// Stop profiling and get results
ProfileInfo profile = DevToolsManager::GetInstance().StopProfiling(page_id);
```

### Remote Debugging

NullWare supports remote debugging for development:

```bash
# Enable remote debugging
./nullware --remote-debugging --devtools-port=9222

# Connect with Chrome DevTools
chrome://inspect/#devices
```

### Keyboard Shortcuts

| Shortcut | Description |
|----------|-------------|
| F12 | Open/Close Developer Tools |
| Ctrl+Shift+I | Open Developer Tools |
| Ctrl+Shift+J | Open Console |
| Ctrl+Shift+N | Open Network Monitor |
| Ctrl+Shift+P | Open Command Menu |
| Ctrl+R | Reload Page |
| Ctrl+Shift+R | Hard Reload |
| F5 | Reload Page |
| Ctrl+F5 | Hard Reload |

## Contributing Guidelines

### Code Style

- Use Google C++ Style Guide
- 4 spaces for indentation
- 80 characters per line maximum
- Descriptive variable and function names
- Comprehensive comments for complex logic

### Commit Messages

- Use imperative mood ("Add feature" not "Added feature")
- First line: 50 characters or less
- Second line: Empty
- Subsequent lines: Wrap at 72 characters

Example:
```
Add ad-blocker toggle functionality

Implement the ability to enable/disable the ad-blocker at runtime
through the settings menu and command line options.

- Add toggle switch in settings dialog
- Add command line flags for ad-blocker control
- Update configuration management
```

### Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests (`npm test`)
5. Run linting (`npm run lint`)
6. Commit your changes
7. Push to the branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

### Code Review

- All PRs require at least one approval
- Address all review comments
- Keep PRs focused and small
- Include tests for new functionality
- Update documentation as needed

## Debugging

### Debug Builds

```bash
# Configure debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with debug symbols
cmake --build . --config Debug
```

### Logging

Enable verbose logging for debugging:

```cpp
// In your code
Logger::GetInstance().SetLevel(LogLevel::VERBOSE);
Logger::GetInstance().SetTarget(LogTarget::CONSOLE);

// Or via command line
./nullware --verbose
```

### Debugging with GDB (Linux)

```bash
# Run in debugger
gdb ./bin/nullware
(gdb) run

# Attach to running process
gdb -p <pid>
```

### Debugging with LLDB (macOS)

```bash
# Run in debugger
lldb ./bin/nullware
(lldb) run

# Attach to running process
lldb -p <pid>
```

### Debugging with Visual Studio (Windows)

1. Open the solution file in Visual Studio
2. Set breakpoints in your code
3. Press F5 to start debugging
4. Use the Visual Studio debugger interface

### Memory Debugging

Use Valgrind on Linux:
```bash
valgrind --leak-check=full --show-leak-kinds=all ./bin/nullware
```

Use AddressSanitizer:
```bash
# Configure with AddressSanitizer
cmake .. -DCMAKE_BUILD_TYPE=Debug -DSANITIZE_ADDRESS=ON

# Build and run
cmake --build .
./bin/nullware
```

## Testing

### Unit Tests

NullWare uses Google Test for unit testing:

```cpp
#include <gtest/gtest.h>

TEST(StringUtilsTest, TrimString) {
    EXPECT_EQ(TrimString("  hello  "), "hello");
    EXPECT_EQ(TrimString("hello"), "hello");
    EXPECT_EQ(TrimString("  "), "");
}

TEST(StringUtilsTest, ToLower) {
    EXPECT_EQ(ToLower("HELLO"), "hello");
    EXPECT_EQ(ToLower("Hello"), "hello");
    EXPECT_EQ(ToLower("hello"), "hello");
}
```

### Running Tests

```bash
# Build tests
cmake .. -DBUILD_TESTS=ON
cmake --build .

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R StringUtilsTest

# Run with verbose output
ctest -V
```

### Integration Tests

Integration tests verify the interaction between components:

```cpp
TEST(AdBlockerIntegrationTest, BlockMaliciousUrl) {
    AdBlocker ad_blocker;
    ad_blocker.Initialize();
    
    ad_blocker.AddCustomFilter("||malicious-site.com/*");
    
    EXPECT_TRUE(ad_blocker.ShouldBlockUrl("http://malicious-site.com/ads.js"));
    EXPECT_FALSE(ad_blocker.ShouldBlockUrl("http://safe-site.com/content.js"));
}
```

### Performance Tests

Performance tests ensure the browser meets performance requirements:

```cpp
TEST(PerformanceTest, FilterMatchingSpeed) {
    FilterEngine engine;
    
    // Load a large filter list
    engine.LoadFilterListFromFile("easylist.txt");
    
    // Time multiple URL checks
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; i++) {
        engine.Matches("http://example.com/ads/banner.js");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete 10,000 checks in under 100ms
    EXPECT_LT(duration.count(), 100);
}
```

## Additional Resources

- [CEF Documentation](https://bitbucket.org/chromiumembedded/cef/src/master/)
- [Chromium Documentation](https://www.chromium.org/developers/)
- [EasyList Documentation](https://easylist.to/)
- [Web Security Best Practices](https://owasp.org/www-project-cheat-sheets/)

## Support

- **Issues**: Report bugs and feature requests on GitHub
- **Discussions**: Join the discussion forum
- **Contributing**: See CONTRIBUTING.md for contribution guidelines
- **License**: MIT License - see LICENSE file for details

---

*This documentation is a work in progress. Please help improve it by contributing updates and corrections.*
