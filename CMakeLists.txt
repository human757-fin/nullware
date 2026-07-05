cmake_minimum_required(VERSION 3.15)

# Project configuration
project(nullware VERSION 1.0.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS 1)
    add_definitions(-DNULLWARE_PLATFORM_WINDOWS=1)
elif(APPLE)
    set(PLATFORM_MACOS 1)
    add_definitions(-DNULLWARE_PLATFORM_MACOS=1)
elif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX 1)
    add_definitions(-DNULLWARE_PLATFORM_LINUX=1)
endif()

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/src/browser
    ${CMAKE_SOURCE_DIR}/src/renderer
    ${CMAKE_SOURCE_DIR}/src/security
    ${CMAKE_SOURCE_DIR}/src/adblocker
    ${CMAKE_SOURCE_DIR}/src/devtools
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/main
    ${CMAKE_SOURCE_DIR}/third_party
)

# Options
option(BUILD_TESTS "Build test suites" ON)
option(ENABLE_ADBLOCKER "Enable built-in ad-blocker" ON)
option(ENABLE_DEVTOOLS "Enable developer tools" ON)
option(ENABLE_SANDBOX "Enable sandboxing" ON)
option(USE_SYSTEM_CEF "Use system-wide CEF installation" OFF)

# Find required packages
find_package(Threads REQUIRED)

# CEF (Chromium Embedded Framework) configuration
if(NOT USE_SYSTEM_CEF)
    # Use bundled CEF
    set(CEF_ROOT ${CMAKE_SOURCE_DIR}/third_party/cef)
    set(CEF_INCLUDE_DIRS ${CEF_ROOT}/include)
    if(EXISTS ${CEF_ROOT}/include/cef_base.h)
        message(STATUS "Using bundled CEF from: ${CEF_ROOT}")
    else()
        message(WARNING "CEF not found at ${CEF_ROOT}. Please run tools/clone_deps.py first.")
    endif()
else()
    # Use system CEF
    find_path(CEF_INCLUDE_DIRS NAMES cef_base.h PATHS /usr/include/cef /usr/local/include/cef)
    if(NOT CEF_INCLUDE_DIRS)
        message(FATAL_ERROR "CEF not found. Please install CEF or set USE_SYSTEM_CEF=OFF")
    endif()
endif()

# Source files
set(COMMON_SOURCES
    src/common/config.cpp
    src/common/logging.cpp
    src/common/string_utils.cpp
)

set(BROWSER_SOURCES
    src/browser/browser_main.cpp
    src/browser/browser_window.cpp
    src/browser/browser_client.cpp
    src/browser/render_handler.cpp
    src/browser/load_handler.cpp
)

set(ADBLOCKER_SOURCES
    src/adblocker/ad_blocker.cpp
    src/adblocker/filter_engine.cpp
    src/adblocker/request_interceptor.cpp
)

set(SECURITY_SOURCES
    src/security/security_manager.cpp
)

set(UI_SOURCES
    src/ui/main_window.cpp
    src/ui/toolbar.cpp
    src/ui/tab_strip.cpp
    src/ui/address_bar.cpp
    src/ui/settings_dialog.cpp
    src/ui/download_manager.cpp
)

set(MAIN_SOURCES
    src/main/main.cpp
)

# Main executable
add_executable(nullware
    ${COMMON_SOURCES}
    ${BROWSER_SOURCES}
    ${ADBLOCKER_SOURCES}
    ${SECURITY_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)

# Conditional compilation for features
if(ENABLE_ADBLOCKER)
    add_definitions(-DENABLE_ADBLOCKER=1)
else()
    add_definitions(-DENABLE_ADBLOCKER=0)
endif()

if(ENABLE_DEVTOOLS)
    add_definitions(-DENABLE_DEVTOOLS=1)
else()
    add_definitions(-DENABLE_DEVTOOLS=0)
endif()

if(ENABLE_SANDBOX)
    add_definitions(-DENABLE_SANDBOX=1)
else()
    add_definitions(-DENABLE_SANDBOX=0)
endif()

# Link libraries
target_link_libraries(nullware
    ${CMAKE_THREAD_LIBS_INIT}
)

# CEF libraries
if(CEF_INCLUDE_DIRS)
    target_include_directories(nullware PRIVATE ${CEF_INCLUDE_DIRS})
    
    # Find CEF libraries
    if(WIN32)
        find_library(CEF_LIBRARY NAMES libcef libcef_dll_wrapper PATHS ${CEF_ROOT}/lib/${CMAKE_BUILD_TYPE})
        if(CEF_LIBRARY)
            target_link_libraries(nullware ${CEF_LIBRARY})
        endif()
    elseif(UNIX AND NOT APPLE)
        find_library(CEF_LIBRARY NAMES cef PATHS ${CEF_ROOT}/lib/${CMAKE_BUILD_TYPE})
        if(CEF_LIBRARY)
            target_link_libraries(nullware ${CEF_LIBRARY})
        endif()
    elseif(APPLE)
        find_library(CEF_LIBRARY NAMES cef PATHS ${CEF_ROOT}/lib/${CMAKE_BUILD_TYPE})
        if(CEF_LIBRARY)
            target_link_libraries(nullware ${CEF_LIBRARY})
        endif()
    endif()
endif()

# Platform-specific configurations
if(WIN32)
    target_link_libraries(nullware
        user32
        gdi32
        winmm
        imm32
        ole32
        oleaut32
        shell32
        version
        comctl32
        advapi32
    )
    
    # Disable console for release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(nullware PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
    
elif(UNIX AND NOT APPLE)
    target_link_libraries(nullware
        X11
        Xcomposite
        Xdamage
        Xext
        Xfixes
        Xi
        Xrandr
        Xrender
        Xtst
        asound
        cups
        dbus
        expat
        fontconfig
        freetype
        glib-2.0
        gobject-2.0
        gtk-3
        nss3
        nspr4
        pango-1.0
        pangocairo-1.0
        atk-1.0
        cairo
        gdk_pixbuf-2.0
    )
    
    # Install to /usr/local/bin
    install(TARGETS nullware DESTINATION bin)
    
elif(APPLE)
    target_link_libraries(nullware
        Cocoa
        CoreFoundation
        CoreGraphics
        CoreText
        Security
        SystemConfiguration
        AppKit
        Foundation
    )
endif()

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -DDEBUG -D_DEBUG)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_compile_options(-O2 -DNDEBUG)
    add_definitions(-DNDEBUG)
endif()

# Warning flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable)
endif()

# Security flags
add_compile_options(
    -fstack-protector-strong
    -fPIE
    -D_FORTIFY_SOURCE=2
)

if(MSVC)
    add_compile_options(/DYNAMICBASE /NXCOMPAT)
endif()

# Build tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS nullware DESTINATION bin)
install(DIRECTORY resources/ DESTINATION share/nullware)
install(DIRECTORY docs/ DESTINATION share/doc/nullware)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "NullWare")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Human757")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Security and developer-friendly web browser")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/human757-fin/nullware")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
include(CPack)

# Print configuration summary
message(STATUS "NullWare Browser Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Ad-Blocker: ${ENABLE_ADBLOCKER}")
message(STATUS "  DevTools: ${ENABLE_DEVTOOLS}")
message(STATUS "  Sandbox: ${ENABLE_SANDBOX}")
message(STATUS "  System CEF: ${USE_SYSTEM_CEF}")
if(CEF_INCLUDE_DIRS)
    message(STATUS "  CEF Include: ${CEF_INCLUDE_DIRS}")
endif()
