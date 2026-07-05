#ifndef NULLWARE_UI_MAIN_WINDOW_H_
#define NULLWARE_UI_MAIN_WINDOW_H_

#include <include/cef_base.h>
#include <include/cef_browser.h>

#include <string>
#include <vector>
#include <memory>

#include "src/browser/browser_client.h"
#include "src/common/config.h"

namespace nullware {

class TabStrip;
class AddressBar;
class Toolbar;
class SettingsDialog;
class DownloadManager;

// Main application window
class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    // Initialize the window
    bool Initialize(BrowserClient* client);
    
    // Show the window
    void Show();
    
    // Hide the window
    void Hide();
    
    // Close the window
    void Close();
    
    // Create a new tab
    void CreateNewTab(const std::string& url = "");
    
    // Close a tab
    void CloseTab(int index);
    
    // Switch to a tab
    void SwitchToTab(int index);
    
    // Get current tab count
    size_t GetTabCount() const;
    
    // Load URL in current tab
    void LoadUrl(const std::string& url);
    
    // Get the browser client
    BrowserClient* GetClient();
    
    // Toggle ad-blocker
    void ToggleAdBlocker();
    
    // Toggle developer tools
    void ToggleDevTools();
    
    // Toggle HTTPS-only mode
    void ToggleHttpsOnly();
    
    // Toggle sandbox
    void ToggleSandbox();
    
    // Open settings dialog
    void OpenSettings();
    
    // Open downloads dialog
    void OpenDownloads();
    
    // Get download manager
    DownloadManager* GetDownloadManager();
    
    // Update UI state
    void UpdateUI();
    
    // Handle browser events
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
    void OnBrowserClosed(CefRefPtr<CefBrowser> browser);
    void OnLoadingStateChange(bool is_loading, bool can_go_back, bool can_go_forward);
    void OnLoadStart(const std::string& url);
    void OnLoadEnd(const std::string& url, int http_status_code);
    void OnLoadError(const std::string& url, int error_code, const std::string& error_text);
    
private:
    BrowserClient* client_;
    std::unique_ptr<TabStrip> tab_strip_;
    std::unique_ptr<AddressBar> address_bar_;
    std::unique_ptr<Toolbar> toolbar_;
    std::unique_ptr<SettingsDialog> settings_dialog_;
    std::unique_ptr<DownloadManager> download_manager_;
    
    std::vector<CefRefPtr<CefBrowser>> browsers_;
    int current_tab_index_;
    
    // UI state
    bool is_visible_;
    bool is_maximized_;
    bool is_minimized_;
    bool is_fullscreen_;
    
    // Settings state
    bool adblocker_enabled_;
    bool devtools_enabled_;
    bool https_only_enabled_;
    bool sandbox_enabled_;
};

}  // namespace nullware

#endif  // NULLWARE_UI_MAIN_WINDOW_H_
