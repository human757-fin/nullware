#ifndef NULLWARE_DEVTOOLS_DEVTOOLS_MANAGER_H_
#define NULLWARE_DEVTOOLS_DEVTOOLS_MANAGER_H_

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>

#include "src/common/config.h"

namespace nullware {

class Inspector;
class NetworkMonitor;
class Console;
class Profiler;

// Developer tools types
enum class DevToolsType {
    INSPECTOR,
    NETWORK,
    CONSOLE,
    PROFILER,
    MEMORY,
    STORAGE,
    APPLICATION,
    SECURITY,
    AUDITS,
    PERFORMANCE
};

// Developer tools panel information
struct DevToolsPanel {
    DevToolsType type;
    std::string name;
    std::string title;
    bool enabled;
    bool visible;
};

// Network request information for debugging
struct NetworkRequestInfo {
    std::string request_id;
    std::string url;
    std::string method;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    std::string response_status;
    std::vector<std::pair<std::string, std::string>> response_headers;
    std::string response_body;
    int64_t start_time;
    int64_t end_time;
    bool from_cache;
};

// Console message
struct ConsoleMessage {
    std::string message;
    std::string source;
    int line;
    std::string level;  // "log", "info", "warning", "error"
    std::string timestamp;
};

// Profile information
struct ProfileInfo {
    std::string name;
    int64_t start_time;
    int64_t end_time;
    std::vector<std::pair<std::string, int64_t>> entries;
};

// Developer tools manager
class DevToolsManager {
public:
    DevToolsManager();
    ~DevToolsManager();
    
    // Initialize developer tools
    bool Initialize();
    
    // Shutdown developer tools
    void Shutdown();
    
    // Enable or disable developer tools
    void SetEnabled(bool enabled);
    bool IsEnabled() const;
    
    // Open developer tools for a specific page
    void OpenDevTools(const std::string& page_id, DevToolsType type = DevToolsType::INSPECTOR);
    
    // Close developer tools for a specific page
    void CloseDevTools(const std::string& page_id);
    
    // Toggle developer tools for a specific page
    void ToggleDevTools(const std::string& page_id);
    
    // Check if developer tools are open for a page
    bool AreDevToolsOpen(const std::string& page_id) const;
    
    // Get the inspector for a page
    Inspector* GetInspector(const std::string& page_id);
    
    // Get the network monitor for a page
    NetworkMonitor* GetNetworkMonitor(const std::string& page_id);
    
    // Get the console for a page
    Console* GetConsole(const std::string& page_id);
    
    // Get the profiler for a page
    Profiler* GetProfiler(const std::string& page_id);
    
    // Log a message to the console
    void LogToConsole(const std::string& page_id, const std::string& message,
                     const std::string& level = "log", const std::string& source = "", int line = 0);
    
    // Record a network request
    void RecordNetworkRequest(const std::string& page_id, const NetworkRequestInfo& request);
    
    // Start profiling
    void StartProfiling(const std::string& page_id, const std::string& name = "");
    
    // Stop profiling
    ProfileInfo StopProfiling(const std::string& page_id);
    
    // Get available panels
    const std::vector<DevToolsPanel>& GetAvailablePanels() const;
    
    // Enable or disable a specific panel
    void SetPanelEnabled(DevToolsType type, bool enabled);
    
    // Get remote debugging URL
    std::string GetRemoteDebuggingUrl() const;
    
    // Enable or disable remote debugging
    void SetRemoteDebugging(bool enabled);
    bool IsRemoteDebuggingEnabled() const;
    
    // Get statistics
    struct DevToolsStatistics {
        size_t pages_with_devtools;
        size_t console_messages;
        size_t network_requests;
        size_t profiles_created;
    };
    
    DevToolsStatistics GetStatistics() const;
    
    // Reset statistics
    void ResetStatistics();
    
private:
    bool enabled_;
    bool remote_debugging_;
    int remote_debugging_port_;
    
    std::vector<DevToolsPanel> available_panels_;
    std::unordered_map<std::string, std::unique_ptr<Inspector>> inspectors_;
    std::unordered_map<std::string, std::unique_ptr<NetworkMonitor>> network_monitors_;
    std::unordered_map<std::string, std::unique_ptr<Console>> consoles_;
    std::unordered_map<std::string, std::unique_ptr<Profiler>> profilers_;
    
    DevToolsStatistics statistics_;
    mutable std::mutex mutex_;
    
    // Initialize available panels
    void InitializeAvailablePanels();
    
    // Create inspector for a page
    Inspector* CreateInspector(const std::string& page_id);
    
    // Create network monitor for a page
    NetworkMonitor* CreateNetworkMonitor(const std::string& page_id);
    
    // Create console for a page
    Console* CreateConsole(const std::string& page_id);
    
    // Create profiler for a page
    Profiler* CreateProfiler(const std::string& page_id);
};

// Global developer tools manager instance
class DevToolsManagerSingleton {
public:
    static DevToolsManager& GetInstance() {
        static DevToolsManager instance;
        return instance;
    }
    
private:
        DevToolsManagerSingleton() = default;
        ~DevToolsManagerSingleton() = default;
};

}  // namespace nullware

#endif  // NULLWARE_DEVTOOLS_DEVTOOLS_MANAGER_H_
