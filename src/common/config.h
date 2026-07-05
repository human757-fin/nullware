#ifndef NULLWARE_COMMON_CONFIG_H_
#define NULLWARE_COMMON_CONFIG_H_

#include <string>
#include <vector>
#include <memory>

// Version information
#define NULLWARE_VERSION_MAJOR 1
#define NULLWARE_VERSION_MINOR 0
#define NULLWARE_VERSION_PATCH 0
#define NULLWARE_VERSION_STRING "1.0.0"

// Platform detection
#if defined(_WIN32)
#define NULLWARE_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#define NULLWARE_PLATFORM_MACOS 1
#elif defined(__linux__)
#define NULLWARE_PLATFORM_LINUX 1
#endif

// Feature flags (can be overridden via CMake)
#ifndef ENABLE_ADBLOCKER
#define ENABLE_ADBLOCKER 1
#endif

#ifndef ENABLE_DEVTOOLS
#define ENABLE_DEVTOOLS 1
#endif

#ifndef ENABLE_SANDBOX
#define ENABLE_SANDBOX 1
#endif

// Default paths
#define DEFAULT_USER_DATA_DIR "nullware_data"
#define DEFAULT_CACHE_DIR "cache"
#define DEFAULT_COOKIE_FILE "cookies.dat"
#define DEFAULT_BOOKMARKS_FILE "bookmarks.json"
#define DEFAULT_HISTORY_FILE "history.json"
#define DEFAULT_SETTINGS_FILE "settings.json"

// Default filter lists for ad-blocker
#define DEFAULT_FILTER_LIST_URL "https://easylist.to/easylist/easylist.txt"
#define DEFAULT_PRIVACY_FILTER_LIST_URL "https://easylist.to/easylist/easyprivacy.txt"

// Security settings
#define DEFAULT_HTTPS_ONLY_MODE false
#define DEFAULT_CERTIFICATE_PINNING true
#define DEFAULT_CSP_ENFORCEMENT true
#define DEFAULT_SAFE_BROWSING true

// Performance settings
#define DEFAULT_MAX_CONNECTIONS_PER_HOST 6
#define DEFAULT_MAX_CONNECTIONS_TOTAL 24
#define DEFAULT_DISK_CACHE_SIZE (100 * 1024 * 1024)  // 100MB
#define DEFAULT_MEMORY_CACHE_SIZE (50 * 1024 * 1024)  // 50MB

// Developer settings
#define DEFAULT_DEVTOOLS_PORT 9222
#define DEFAULT_REMOTE_DEBUGGING false

// User agent string
#define NULLWARE_USER_AGENT \
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 " \
    "(KHTML, like Gecko) NullWare/" NULLWARE_VERSION_STRING " Chrome/" \
    "CEF_VERSION Safari/537.36"

// Command line switches
#define SWITCH_DISABLE_ADBLOCKER "--disable-adblocker"
#define SWITCH_ENABLE_ADBLOCKER "--enable-adblocker"
#define SWITCH_DISABLE_DEVTOOLS "--disable-devtools"
#define SWITCH_ENABLE_DEVTOOLS "--enable-devtools"
#define SWITCH_DISABLE_SANDBOX "--disable-sandbox"
#define SWITCH_HTTPS_ONLY "--https-only"
#define SWITCH_INCOGNITO "--incognito"
#define SWITCH_REMOTE_DEBUGGING "--remote-debugging"
#define SWITCH_FILTER_LIST "--filter-list"
#define SWITCH_USER_DATA_DIR "--user-data-dir"
#define SWITCH_PROXY_SERVER "--proxy-server"

// Security constants
#define MAX_PASSWORD_LENGTH 256
#define MAX_URL_LENGTH 2048
#define MAX_COOKIE_SIZE 4096
#define MAX_HEADER_SIZE 8192

// Network timeouts (in milliseconds)
#define NETWORK_CONNECT_TIMEOUT 30000
#define NETWORK_READ_TIMEOUT 60000
#define NETWORK_WRITE_TIMEOUT 60000

// File paths
#define PATH_SEPARATOR \
    NULLWARE_PLATFORM_WINDOWS ? "\\" : "/"

namespace nullware {

// Configuration structure
struct BrowserConfig {
    // Ad-blocker settings
    bool enable_adblocker = ENABLE_ADBLOCKER;
    std::vector<std::string> filter_list_urls = {
        DEFAULT_FILTER_LIST_URL,
        DEFAULT_PRIVACY_FILTER_LIST_URL
    };
    std::vector<std::string> custom_filter_files;
    
    // Security settings
    bool https_only_mode = DEFAULT_HTTPS_ONLY_MODE;
    bool certificate_pinning = DEFAULT_CERTIFICATE_PINNING;
    bool csp_enforcement = DEFAULT_CSP_ENFORCEMENT;
    bool safe_browsing = DEFAULT_SAFE_BROWSING;
    bool sandbox_enabled = ENABLE_SANDBOX;
    
    // Developer settings
    bool enable_devtools = ENABLE_DEVTOOLS;
    bool remote_debugging = DEFAULT_REMOTE_DEBUGGING;
    int devtools_port = DEFAULT_DEVTOOLS_PORT;
    
    // Performance settings
    int max_connections_per_host = DEFAULT_MAX_CONNECTIONS_PER_HOST;
    int max_connections_total = DEFAULT_MAX_CONNECTIONS_TOTAL;
    size_t disk_cache_size = DEFAULT_DISK_CACHE_SIZE;
    size_t memory_cache_size = DEFAULT_MEMORY_CACHE_SIZE;
    
    // Path settings
    std::string user_data_dir = DEFAULT_USER_DATA_DIR;
    std::string cache_dir = DEFAULT_CACHE_DIR;
    
    // Proxy settings
    std::string proxy_server;
    bool use_proxy = false;
    
    // Privacy settings
    bool do_not_track = true;
    bool block_third_party_cookies = false;
    bool clear_cookies_on_exit = false;
    bool clear_cache_on_exit = false;
    
    // UI settings
    bool show_toolbar = true;
    bool show_status_bar = true;
    bool show_bookmarks_bar = false;
    std::string default_homepage = "about:blank";
    std::string startup_pages = "about:blank";
    
    // Experimental features
    bool experimental_webgl = true;
    bool experimental_webrtc = false;
    bool experimental_webassembly = true;
};

// Global configuration instance
class ConfigManager {
public:
    static ConfigManager& GetInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    BrowserConfig& GetConfig() { return config_; }
    const BrowserConfig& GetConfig() const { return config_; }
    
    void SetConfig(const BrowserConfig& config) { config_ = config; }
    void UpdateConfig(const BrowserConfig& partial_config);
    
    void LoadFromFile(const std::string& path);
    void SaveToFile(const std::string& path) const;
    void LoadFromCommandLine(int argc, char* argv[]);
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    BrowserConfig config_;
};

}  // namespace nullware

#endif  // NULLWARE_COMMON_CONFIG_H_
