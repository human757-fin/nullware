#ifndef NULLWARE_UI_TOOLBAR_H_
#define NULLWARE_UI_TOOLBAR_H_

#include <string>
#include <functional>

namespace nullware {

class MainWindow;

// Toolbar button types
enum class ToolbarButton {
    BACK,
    FORWARD,
    REFRESH,
    HOME,
    NEW_TAB,
    ADBLOCKER,
    DEVTOOLS,
    HTTPS_ONLY,
    SANDBOX,
    DOWNLOADS,
    SETTINGS
};

// Toolbar class
class Toolbar {
public:
    Toolbar(MainWindow* main_window);
    ~Toolbar();
    
    // Initialize the toolbar
    bool Initialize();
    
    // Show the toolbar
    void Show();
    
    // Hide the toolbar
    void Hide();
    
    // Update button states
    void UpdateNavigationButtons(bool can_go_back, bool can_go_forward);
    void UpdateLoadingIndicator(bool is_loading);
    void UpdateAdBlockerToggle(bool enabled);
    void UpdateDevToolsToggle(bool enabled);
    void UpdateHttpsOnlyToggle(bool enabled);
    void UpdateSandboxToggle(bool enabled);
    
    // Button click handlers
    void OnBackClicked();
    void OnForwardClicked();
    void OnRefreshClicked();
    void OnHomeClicked();
    void OnNewTabClicked();
    void OnAdBlockerClicked();
    void OnDevToolsClicked();
    void OnHttpsOnlyClicked();
    void OnSandboxClicked();
    void OnDownloadsClicked();
    void OnSettingsClicked();
    
private:
    MainWindow* main_window_;
    
    // Button states
    bool can_go_back_;
    bool can_go_forward_;
    bool is_loading_;
    bool adblocker_enabled_;
    bool devtools_enabled_;
    bool https_only_enabled_;
    bool sandbox_enabled_;
    
    // Platform-specific implementation
    void* native_handle_;
};

}  // namespace nullware

#endif  // NULLWARE_UI_TOOLBAR_H_
