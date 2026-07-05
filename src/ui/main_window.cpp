#include "src/ui/main_window.h"

#include "src/ui/tab_strip.h"
#include "src/ui/address_bar.h"
#include "src/ui/toolbar.h"
#include "src/ui/settings_dialog.h"
#include "src/ui/download_manager.h"
#include "src/adblocker/ad_blocker.h"
#include "src/common/logging.h"
#include "src/common/config.h"

// Platform-specific includes
#ifdef NULLWARE_PLATFORM_WINDOWS
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#elif defined(NULLWARE_PLATFORM_LINUX)
#include <gtk/gtk.h>
#elif defined(NULLWARE_PLATFORM_MACOS)
#include <Cocoa/Cocoa.h>
#endif

namespace nullware {

MainWindow::MainWindow() 
    : client_(nullptr),
      current_tab_index_(0),
      is_visible_(false),
      is_maximized_(false),
      is_minimized_(false),
      is_fullscreen_(false),
      adblocker_enabled_(true),
      devtools_enabled_(true),
      https_only_enabled_(false),
      sandbox_enabled_(true) {
    
    // Initialize UI components
    tab_strip_ = std::make_unique<TabStrip>(this);
    address_bar_ = std::make_unique<AddressBar>(this);
    toolbar_ = std::make_unique<Toolbar>(this);
    settings_dialog_ = std::make_unique<SettingsDialog>(this);
    download_manager_ = std::make_unique<DownloadManager>(this);
}

MainWindow::~MainWindow() {
    // Clean up browsers
    for (auto& browser : browsers_) {
        if (browser) {
            browser->GetHost()->CloseBrowser(true);
        }
    }
    browsers_.clear();
}

bool MainWindow::Initialize(BrowserClient* client) {
    LOG_INFO("Initializing MainWindow");
    
    client_ = client;
    
    // Load settings from config
    const auto& config = ConfigManager::GetInstance().GetConfig();
    adblocker_enabled_ = config.enable_adblocker;
    devtools_enabled_ = config.enable_devtools;
    https_only_enabled_ = config.https_only_mode;
    sandbox_enabled_ = config.sandbox_enabled;
    
    // Initialize UI components
    if (!tab_strip_->Initialize()) {
        LOG_ERROR("Failed to initialize tab strip");
        return false;
    }
    
    if (!address_bar_->Initialize()) {
        LOG_ERROR("Failed to initialize address bar");
        return false;
    }
    
    if (!toolbar_->Initialize()) {
        LOG_ERROR("Failed to initialize toolbar");
        return false;
    }
    
    if (!settings_dialog_->Initialize()) {
        LOG_ERROR("Failed to initialize settings dialog");
        return false;
    }
    
    if (!download_manager_->Initialize()) {
        LOG_ERROR("Failed to initialize download manager");
        return false;
    }
    
    // Create first tab
    CreateNewTab("about:blank");
    
    LOG_INFO("MainWindow initialized successfully");
    return true;
}

void MainWindow::Show() {
    if (is_visible_) return;
    
    LOG_INFO("Showing MainWindow");
    is_visible_ = true;
    
    // Show all UI components
    tab_strip_->Show();
    address_bar_->Show();
    toolbar_->Show();
}

void MainWindow::Hide() {
    if (!is_visible_) return;
    
    LOG_INFO("Hiding MainWindow");
    is_visible_ = false;
    
    // Hide all UI components
    tab_strip_->Hide();
    address_bar_->Hide();
    toolbar_->Hide();
}

void MainWindow::Close() {
    LOG_INFO("Closing MainWindow");
    
    // Close all tabs
    while (browsers_.size() > 0) {
        CloseTab(0);
    }
    
    is_visible_ = false;
}

void MainWindow::CreateNewTab(const std::string& url) {
    LOG_INFO("Creating new tab with URL: " + url);
    
    if (client_) {
        // Create browser for the new tab
        CefWindowInfo window_info;
        window_info.SetAsWindowless(nullptr);
        
        CefBrowserSettings browser_settings;
        
        // Configure browser settings based on current settings
        browser_settings.webgl = true;
        
        CefBrowserHost::CreateBrowser(window_info, client_, 
                                      CefString(url.empty() ? "about:blank" : url), 
                                      browser_settings, nullptr);
    }
    
    // Update tab strip
    tab_strip_->AddTab(url.empty() ? "New Tab" : ExtractDomain(url));
    current_tab_index_ = tab_strip_->GetTabCount() - 1;
    tab_strip_->SetActiveTab(current_tab_index_);
    
    // Update address bar
    address_bar_->SetUrl(url);
}

void MainWindow::CloseTab(int index) {
    if (index < 0 || index >= static_cast<int>(browsers_.size())) {
        return;
    }
    
    LOG_INFO("Closing tab at index: " + std::to_string(index));
    
    // Close the browser
    if (browsers_[index]) {
        browsers_[index]->GetHost()->CloseBrowser(true);
    }
    
    // Remove from browsers list
    browsers_.erase(browsers_.begin() + index);
    
    // Update tab strip
    tab_strip_->RemoveTab(index);
    
    // Adjust current tab index
    if (current_tab_index_ >= index) {
        current_tab_index_ = std::max(0, current_tab_index_ - 1);
    }
    
    // Switch to another tab if available
    if (browsers_.size() > 0) {
        SwitchToTab(current_tab_index_);
    } else {
        // No tabs left, create a new one
        CreateNewTab("about:blank");
    }
}

void MainWindow::SwitchToTab(int index) {
    if (index < 0 || index >= static_cast<int>(browsers_.size())) {
        return;
    }
    
    LOG_INFO("Switching to tab at index: " + std::to_string(index));
    
    current_tab_index_ = index;
    tab_strip_->SetActiveTab(index);
    
    // Update address bar with current tab's URL
    if (browsers_[index]) {
        CefRefPtr<CefFrame> frame = browsers_[index]->GetMainFrame();
        if (frame) {
            address_bar_->SetUrl(frame->GetURL().ToString());
        }
    }
}

size_t MainWindow::GetTabCount() const {
    return browsers_.size();
}

void MainWindow::LoadUrl(const std::string& url) {
    LOG_INFO("Loading URL in current tab: " + url);
    
    if (current_tab_index_ >= 0 && current_tab_index_ < static_cast<int>(browsers_.size())) {
        CefRefPtr<CefFrame> frame = browsers_[current_tab_index_]->GetMainFrame();
        if (frame) {
            frame->LoadURL(CefString(url));
            address_bar_->SetUrl(url);
        }
    }
}

BrowserClient* MainWindow::GetClient() {
    return client_;
}

void MainWindow::ToggleAdBlocker() {
    adblocker_enabled_ = !adblocker_enabled_;
    
    // Update ad-blocker
    auto ad_blocker = AdBlockerManager::GetInstance().GetAdBlocker();
    if (ad_blocker) {
        ad_blocker->SetEnabled(adblocker_enabled_);
    }
    
    // Update config
    auto config = ConfigManager::GetInstance().GetConfig();
    config.enable_adblocker = adblocker_enabled_;
    ConfigManager::GetInstance().SetConfig(config);
    
    // Update UI
    toolbar_->UpdateAdBlockerToggle(adblocker_enabled_);
    
    LOG_INFO("Ad-blocker " + std::string(adblocker_enabled_ ? "enabled" : "disabled"));
}

void MainWindow::ToggleDevTools() {
    devtools_enabled_ = !devtools_enabled_;
    
    // Update config
    auto config = ConfigManager::GetInstance().GetConfig();
    config.enable_devtools = devtools_enabled_;
    ConfigManager::GetInstance().SetConfig(config);
    
    // Update UI
    toolbar_->UpdateDevToolsToggle(devtools_enabled_);
    
    LOG_INFO("Developer tools " + std::string(devtools_enabled_ ? "enabled" : "disabled"));
}

void MainWindow::ToggleHttpsOnly() {
    https_only_enabled_ = !https_only_enabled_;
    
    // Update security manager
    auto security_manager = SecurityManagerSingleton::GetInstance();
    security_manager.SetHttpsOnlyMode(https_only_enabled_);
    
    // Update config
    auto config = ConfigManager::GetInstance().GetConfig();
    config.https_only_mode = https_only_enabled_;
    ConfigManager::GetInstance().SetConfig(config);
    
    // Update UI
    toolbar_->UpdateHttpsOnlyToggle(https_only_enabled_);
    
    LOG_INFO("HTTPS-only mode " + std::string(https_only_enabled_ ? "enabled" : "disabled"));
}

void MainWindow::ToggleSandbox() {
    sandbox_enabled_ = !sandbox_enabled_;
    
    // Update config
    auto config = ConfigManager::GetInstance().GetConfig();
    config.sandbox_enabled = sandbox_enabled_;
    ConfigManager::GetInstance().SetConfig(config);
    
    // Update UI
    toolbar_->UpdateSandboxToggle(sandbox_enabled_);
    
    LOG_INFO("Sandbox " + std::string(sandbox_enabled_ ? "enabled" : "disabled"));
}

void MainWindow::OpenSettings() {
    LOG_INFO("Opening settings dialog");
    settings_dialog_->Show();
}

void MainWindow::OpenDownloads() {
    LOG_INFO("Opening downloads dialog");
    download_manager_->ShowDownloadsDialog();
}

DownloadManager* MainWindow::GetDownloadManager() {
    return download_manager_.get();
}

void MainWindow::UpdateUI() {
    toolbar_->UpdateAdBlockerToggle(adblocker_enabled_);
    toolbar_->UpdateDevToolsToggle(devtools_enabled_);
    toolbar_->UpdateHttpsOnlyToggle(https_only_enabled_);
    toolbar_->UpdateSandboxToggle(sandbox_enabled_);
}

void MainWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser created in MainWindow");
    browsers_.push_back(browser);
    
    // Update tab strip
    if (browsers_.size() == 1) {
        tab_strip_->AddTab("New Tab");
    }
}

void MainWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser closed in MainWindow");
    
    // Remove browser from list
    auto it = std::find(browsers_.begin(), browsers_.end(), browser);
    if (it != browsers_.end()) {
        size_t index = std::distance(browsers_.begin(), it);
        browsers_.erase(it);
        tab_strip_->RemoveTab(static_cast<int>(index));
    }
}

void MainWindow::OnLoadingStateChange(bool is_loading, bool can_go_back, bool can_go_forward) {
    toolbar_->UpdateNavigationButtons(can_go_back, can_go_forward);
    toolbar_->UpdateLoadingIndicator(is_loading);
}

void MainWindow::OnLoadStart(const std::string& url) {
    address_bar_->SetUrl(url);
    toolbar_->UpdateLoadingIndicator(true);
}

void MainWindow::OnLoadEnd(const std::string& url, int http_status_code) {
    address_bar_->SetUrl(url);
    toolbar_->UpdateLoadingIndicator(false);
    
    // Update tab title if possible
    if (current_tab_index_ >= 0 && current_tab_index_ < static_cast<int>(tab_strip_->GetTabCount())) {
        tab_strip_->SetTabTitle(current_tab_index_, ExtractDomain(url));
    }
}

void MainWindow::OnLoadError(const std::string& url, int error_code, const std::string& error_text) {
    toolbar_->UpdateLoadingIndicator(false);
    LOG_ERROR("Load error: " + error_text + " (" + std::to_string(error_code) + ")");
}

}  // namespace nullware
