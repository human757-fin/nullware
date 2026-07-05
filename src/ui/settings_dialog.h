#ifndef NULLWARE_UI_SETTINGS_DIALOG_H_
#define NULLWARE_UI_SETTINGS_DIALOG_H_

#include <string>
#include <functional>

namespace nullware {

class MainWindow;

// Settings dialog tabs
enum class SettingsTab {
    GENERAL,
    PRIVACY,
    SECURITY,
    ADBLOCKER,
    DEVELOPER,
    ADVANCED
};

// Settings dialog class
class SettingsDialog {
public:
    SettingsDialog(MainWindow* main_window);
    ~SettingsDialog();
    
    // Initialize the settings dialog
    bool Initialize();
    
    // Show the settings dialog
    void Show();
    
    // Hide the settings dialog
    void Hide();
    
    // Set active tab
    void SetActiveTab(SettingsTab tab);
    
    // Apply settings
    void ApplySettings();
    
    // Cancel and close
    void Cancel();
    
    // Reset to defaults
    void ResetToDefaults();
    
    // Get current settings
    void LoadCurrentSettings();
    
    // Save settings
    void SaveSettings();
    
private:
    MainWindow* main_window_;
    SettingsTab active_tab_;
    
    // Current settings (temporary storage for editing)
    struct SettingsState {
        // General settings
        std::string homepage;
        bool show_toolbar;
        bool show_status_bar;
        bool show_bookmarks_bar;
        
        // Privacy settings
        bool do_not_track;
        bool block_third_party_cookies;
        bool clear_cookies_on_exit;
        bool clear_cache_on_exit;
        bool clear_history_on_exit;
        
        // Security settings
        bool enable_adblocker;
        bool https_only_mode;
        bool certificate_pinning;
        bool csp_enforcement;
        bool safe_browsing;
        bool sandbox_enabled;
        
        // Ad-blocker settings
        std::vector<std::string> filter_list_urls;
        std::vector<std::string> custom_filter_files;
        
        // Developer settings
        bool enable_devtools;
        bool remote_debugging;
        int devtools_port;
        
        // Advanced settings
        int max_connections_per_host;
        int max_connections_total;
        size_t disk_cache_size;
        size_t memory_cache_size;
        
        // Proxy settings
        bool use_proxy;
        std::string proxy_server;
        
    } current_settings_;
    
    // Original settings (for cancel)
    SettingsState original_settings_;
    
    // Platform-specific implementation
    void* native_handle_;
};

}  // namespace nullware

#endif  // NULLWARE_UI_SETTINGS_DIALOG_H_
