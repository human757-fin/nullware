#include "src/ui/settings_dialog.h"

#include "src/ui/main_window.h"
#include "src/common/logging.h"
#include "src/common/config.h"
#include "src/adblocker/ad_blocker.h"
#include "src/security/security_manager.h"

// Platform-specific includes
#ifdef NULLWARE_PLATFORM_WINDOWS
#include <windows.h>
#include <commctrl.h>
#elif defined(NULLWARE_PLATFORM_LINUX)
#include <gtk/gtk.h>
#elif defined(NULLWARE_PLATFORM_MACOS)
#include <Cocoa/Cocoa.h>
#endif

namespace nullware {

SettingsDialog::SettingsDialog(MainWindow* main_window)
    : main_window_(main_window),
      active_tab_(SettingsTab::GENERAL),
      native_handle_(nullptr) {}

SettingsDialog::~SettingsDialog() {
    // Clean up platform-specific resources
}

bool SettingsDialog::Initialize() {
    LOG_INFO("Initializing SettingsDialog");
    
    // Platform-specific initialization
    #ifdef NULLWARE_PLATFORM_WINDOWS
    // Windows implementation would go here
    #elif defined(NULLWARE_PLATFORM_LINUX)
    // GTK implementation would go here
    #elif defined(NULLWARE_PLATFORM_MACOS)
    // macOS implementation would go here
    #endif
    
    return true;
}

void SettingsDialog::Show() {
    LOG_DEBUG("Showing SettingsDialog");
    
    // Load current settings
    LoadCurrentSettings();
    
    // Platform-specific show implementation
}

void SettingsDialog::Hide() {
    LOG_DEBUG("Hiding SettingsDialog");
    // Platform-specific hide implementation
}

void SettingsDialog::SetActiveTab(SettingsTab tab) {
    LOG_DEBUG("Setting active settings tab: " + std::to_string(static_cast<int>(tab)));
    active_tab_ = tab;
}

void SettingsDialog::ApplySettings() {
    LOG_INFO("Applying settings");
    SaveSettings();
    Hide();
}

void SettingsDialog::Cancel() {
    LOG_INFO("Cancelling settings changes");
    current_settings_ = original_settings_;
    Hide();
}

void SettingsDialog::ResetToDefaults() {
    LOG_INFO("Resetting settings to defaults");
    
    // Reset to default values
    current_settings_ = SettingsState();
    
    // Set default values
    current_settings_.homepage = "about:blank";
    current_settings_.show_toolbar = true;
    current_settings_.show_status_bar = true;
    current_settings_.show_bookmarks_bar = false;
    current_settings_.do_not_track = true;
    current_settings_.block_third_party_cookies = false;
    current_settings_.clear_cookies_on_exit = false;
    current_settings_.clear_cache_on_exit = false;
    current_settings_.clear_history_on_exit = false;
    current_settings_.enable_adblocker = true;
    current_settings_.https_only_mode = false;
    current_settings_.certificate_pinning = true;
    current_settings_.csp_enforcement = true;
    current_settings_.safe_browsing = true;
    current_settings_.sandbox_enabled = true;
    current_settings_.enable_devtools = true;
    current_settings_.remote_debugging = false;
    current_settings_.devtools_port = 9222;
    current_settings_.max_connections_per_host = 6;
    current_settings_.max_connections_total = 24;
    current_settings_.disk_cache_size = 100 * 1024 * 1024; // 100MB
    current_settings_.memory_cache_size = 50 * 1024 * 1024; // 50MB
    current_settings_.use_proxy = false;
    current_settings_.proxy_server = "";
    
    // Add default filter lists
    current_settings_.filter_list_urls = {
        DEFAULT_FILTER_LIST_URL,
        DEFAULT_PRIVACY_FILTER_LIST_URL
    };
}

void SettingsDialog::LoadCurrentSettings() {
    LOG_INFO("Loading current settings");
    
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    // Load current settings
    current_settings_.homepage = config.default_homepage;
    current_settings_.show_toolbar = config.show_toolbar;
    current_settings_.show_status_bar = config.show_status_bar;
    current_settings_.show_bookmarks_bar = config.show_bookmarks_bar;
    current_settings_.do_not_track = config.do_not_track;
    current_settings_.block_third_party_cookies = config.block_third_party_cookies;
    current_settings_.clear_cookies_on_exit = config.clear_cookies_on_exit;
    current_settings_.clear_cache_on_exit = config.clear_cache_on_exit;
    current_settings_.enable_adblocker = config.enable_adblocker;
    current_settings_.https_only_mode = config.https_only_mode;
    current_settings_.certificate_pinning = config.certificate_pinning;
    current_settings_.csp_enforcement = config.csp_enforcement;
    current_settings_.safe_browsing = config.safe_browsing;
    current_settings_.sandbox_enabled = config.sandbox_enabled;
    current_settings_.enable_devtools = config.enable_devtools;
    current_settings_.remote_debugging = config.remote_debugging;
    current_settings_.devtools_port = config.devtools_port;
    current_settings_.max_connections_per_host = config.max_connections_per_host;
    current_settings_.max_connections_total = config.max_connections_total;
    current_settings_.disk_cache_size = config.disk_cache_size;
    current_settings_.memory_cache_size = config.memory_cache_size;
    current_settings_.use_proxy = config.use_proxy;
    current_settings_.proxy_server = config.proxy_server;
    current_settings_.filter_list_urls = config.filter_list_urls;
    current_settings_.custom_filter_files = config.custom_filter_files;
    
    // Save original settings for cancel
    original_settings_ = current_settings_;
}

void SettingsDialog::SaveSettings() {
    LOG_INFO("Saving settings");
    
    // Update config
    BrowserConfig config = ConfigManager::GetInstance().GetConfig();
    
    config.default_homepage = current_settings_.homepage;
    config.show_toolbar = current_settings_.show_toolbar;
    config.show_status_bar = current_settings_.show_status_bar;
    config.show_bookmarks_bar = current_settings_.show_bookmarks_bar;
    config.do_not_track = current_settings_.do_not_track;
    config.block_third_party_cookies = current_settings_.block_third_party_cookies;
    config.clear_cookies_on_exit = current_settings_.clear_cookies_on_exit;
    config.clear_cache_on_exit = current_settings_.clear_cache_on_exit;
    config.enable_adblocker = current_settings_.enable_adblocker;
    config.https_only_mode = current_settings_.https_only_mode;
    config.certificate_pinning = current_settings_.certificate_pinning;
    config.csp_enforcement = current_settings_.csp_enforcement;
    config.safe_browsing = current_settings_.safe_browsing;
    config.sandbox_enabled = current_settings_.sandbox_enabled;
    config.enable_devtools = current_settings_.enable_devtools;
    config.remote_debugging = current_settings_.remote_debugging;
    config.devtools_port = current_settings_.devtools_port;
    config.max_connections_per_host = current_settings_.max_connections_per_host;
    config.max_connections_total = current_settings_.max_connections_total;
    config.disk_cache_size = current_settings_.disk_cache_size;
    config.memory_cache_size = current_settings_.memory_cache_size;
    config.use_proxy = current_settings_.use_proxy;
    config.proxy_server = current_settings_.proxy_server;
    config.filter_list_urls = current_settings_.filter_list_urls;
    config.custom_filter_files = current_settings_.custom_filter_files;
    
    ConfigManager::GetInstance().SetConfig(config);
    
    // Update ad-blocker
    auto ad_blocker = AdBlockerManager::GetInstance().GetAdBlocker();
    if (ad_blocker) {
        ad_blocker->SetEnabled(config.enable_adblocker);
        
        // Reload filter lists if they changed
        if (config.filter_list_urls != original_settings_.filter_list_urls ||
            config.custom_filter_files != original_settings_.custom_filter_files) {
            ad_blocker->UpdateFilterLists();
        }
    }
    
    // Update security manager
    auto security_manager = SecurityManagerSingleton::GetInstance();
    security_manager.SetHttpsOnlyMode(config.https_only_mode);
    security_manager.SetCertificatePinning(config.certificate_pinning);
    security_manager.SetCspEnforcement(config.csp_enforcement);
    security_manager.SetSafeBrowsing(config.safe_browsing);
    
    // Update main window UI
    if (main_window_) {
        main_window_->UpdateUI();
    }
    
    LOG_INFO("Settings saved successfully");
}

}  // namespace nullware
