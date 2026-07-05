# NullWare Browser - UI Features Documentation

This document describes all the UI features, toggles, tabs, and download functionality added to NullWare Browser.

## 🎨 **Polished UI Components**

### **1. MainWindow** (`src/ui/main_window.h/cpp`)
The central UI management class that coordinates all UI components and browser functionality.

**Features:**
- Tab management (create, close, switch)
- Browser window lifecycle management
- Integration with all UI components
- Feature toggle handlers
- Event routing to appropriate components

**Key Methods:**
```cpp
// Tab management
void CreateNewTab(const std::string& url = "");
void CloseTab(int index);
void SwitchToTab(int index);
size_t GetTabCount() const;

// Feature toggles
void ToggleAdBlocker();
void ToggleDevTools();
void ToggleHttpsOnly();
void ToggleSandbox();

// UI management
void OpenSettings();
void OpenDownloads();
void UpdateUI();
```

### **2. Toolbar** (`src/ui/toolbar.h/cpp`)
Navigation and feature control toolbar with buttons for all major functions.

**Buttons Available:**
- **Back** - Navigate back in history
- **Forward** - Navigate forward in history
- **Refresh** - Reload current page
- **Home** - Go to homepage
- **New Tab** - Create new tab
- **Ad-Blocker Toggle** - Enable/disable ad-blocker
- **DevTools Toggle** - Enable/disable developer tools
- **HTTPS-Only Toggle** - Enable/disable HTTPS-only mode
- **Sandbox Toggle** - Enable/disable sandboxing
- **Downloads** - Open downloads dialog
- **Settings** - Open settings dialog

**Visual States:**
- Navigation buttons enabled/disabled based on history
- Loading indicator for page load status
- Toggle buttons show on/off state

### **3. TabStrip** (`src/ui/tab_strip.h/cpp`)
Tab management component that displays and manages browser tabs.

**Features:**
- Add new tabs
- Close existing tabs
- Switch between tabs
- Visual active tab indication
- Tab title display
- Loading state indicators
- Favicon support (placeholder)

**Tab Information:**
```cpp
struct TabInfo {
    std::string title;      // Tab title (usually page title or domain)
    std::string url;         // Current URL
    bool is_active;          // Is this the active tab
    bool is_loading;        // Is the page loading
    int favicon_id;          // Favicon identifier
};
```

### **4. AddressBar** (`src/ui/address_bar.h/cpp`)
URL input and display component with security indicators.

**Features:**
- URL display and editing
- Auto-completion (placeholder)
- Security indicators (HTTPS/HTTP)
- Domain highlighting
- Focus management

**Security Indicators:**
- 🔒 **Secure** - HTTPS connection with valid certificate
- ⚠️ **Insecure** - HTTP connection
- 🛡️ **Enhanced** - HTTPS with additional security features

### **5. SettingsDialog** (`src/ui/settings_dialog.h/cpp`)
Comprehensive settings management dialog.

**Settings Tabs:**
1. **General** - Homepage, UI visibility options
2. **Privacy** - Tracking protection, cookie settings
3. **Security** - HTTPS-only, certificate pinning, CSP
4. **Ad-Blocker** - Filter lists, custom filters
5. **Developer** - DevTools, remote debugging
6. **Advanced** - Connection limits, cache sizes, proxy

**Settings Management:**
- Load current settings on open
- Apply changes immediately
- Reset to defaults
- Cancel without saving

### **6. DownloadManager** (`src/ui/download_manager.h/cpp`)
Download handling with progress tracking and management.

**Features:**
- Start downloads from links or programmatically
- Pause and resume downloads
- Cancel downloads
- Progress tracking
- Download history
- Clear completed downloads

**Download Information:**
```cpp
enum class DownloadStatus {
    PENDING,       // Download not started yet
    IN_PROGRESS,   // Download in progress
    COMPLETED,     // Download finished successfully
    CANCELLED,     // Download cancelled by user
    FAILED,        // Download failed
    INTERRUPTED    // Download paused
};

struct DownloadInfo {
    std::string id;             // Unique download ID
    std::string url;            // Source URL
    std::string filename;       // Filename
    std::string save_path;      // Full save path
    int64_t total_bytes;        // Total file size
    int64_t received_bytes;     // Bytes downloaded
    int percent_complete;       // Percentage complete
    DownloadStatus status;      // Current status
    std::string mime_type;      // MIME type
    std::string start_time;     // Start timestamp
    std::string end_time;       // End timestamp
};
```

## 🎛️ **Feature Toggles**

### **UI Toggles (Toolbar Buttons)**

| Button | Icon | Description | Default State |
|--------|------|-------------|---------------|
| Ad-Blocker | 🚫 | Enable/disable ad-blocking | ✅ Enabled |
| DevTools | 👨‍💻 | Enable/disable developer tools | ✅ Enabled |
| HTTPS-Only | 🔒 | Force HTTPS connections only | ❌ Disabled |
| Sandbox | 🛡️ | Enable/disable process sandboxing | ✅ Enabled |

### **Command Line Equivalents**

| UI Toggle | Command Line Option |
|-----------|---------------------|
| Ad-Blocker | `--disable-adblocker` / `--enable-adblocker` |
| DevTools | `--disable-devtools` / `--enable-devtools` |
| HTTPS-Only | `--https-only` |
| Sandbox | `--disable-sandbox` / `--enable-sandbox` |

### **Toggle Implementation**

Each toggle button in the toolbar calls the corresponding method in MainWindow:

```cpp
// In Toolbar::OnAdBlockerClicked()
void Toolbar::OnAdBlockerClicked() {
    if (main_window_) {
        main_window_->ToggleAdBlocker();
    }
}

// In MainWindow::ToggleAdBlocker()
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
}
```

## 📑 **Tab Management**

### **Tab Operations**

| Operation | Method | Description |
|-----------|--------|-------------|
| Create Tab | `CreateNewTab(url)` | Create new tab with optional URL |
| Close Tab | `CloseTab(index)` | Close tab at specified index |
| Switch Tab | `SwitchToTab(index)` | Switch to tab at index |
| Get Count | `GetTabCount()` | Get number of open tabs |

### **Tab Lifecycle**

1. **Creation**
   - New browser instance created
   - Added to browsers_ vector
   - Tab added to TabStrip
   - URL loaded if specified

2. **Activation**
   - Tab becomes active
   - Address bar updated with URL
   - Navigation buttons updated

3. **Closure**
   - Browser instance closed
   - Removed from browsers_ vector
   - Tab removed from TabStrip
   - Switch to adjacent tab or create new one

### **Keyboard Shortcuts (Planned)**

| Shortcut | Action |
|----------|--------|
| Ctrl+T | New Tab |
| Ctrl+W | Close Tab |
| Ctrl+Tab | Next Tab |
| Ctrl+Shift+Tab | Previous Tab |
| Ctrl+1-9 | Switch to Tab 1-9 |

## 📥 **Download Management**

### **Download Operations**

| Operation | Method | Description |
|-----------|--------|-------------|
| Start | `StartDownload(url, filename)` | Begin downloading a file |
| Cancel | `CancelDownload(id)` | Cancel a download |
| Pause | `PauseDownload(id)` | Pause a download |
| Resume | `ResumeDownload(id)` | Resume a paused download |
| Get List | `GetDownloadList()` | Get all downloads |
| Get Info | `GetDownload(id)` | Get specific download info |
| Clear Completed | `ClearCompletedDownloads()` | Remove completed downloads |
| Clear All | `ClearAllDownloads()` | Remove all downloads |

### **Download Flow**

1. **Initiation**
   - User clicks download link or calls StartDownload()
   - CEF's OnBeforeDownload() called
   - Download info created and added to list

2. **Progress**
   - CEF's OnDownloadUpdated() called periodically
   - Progress updated in DownloadManager
   - UI updated with progress percentage

3. **Completion**
   - Download status set to COMPLETED
   - End time recorded
   - Notification shown to user

### **Download Status Transitions**

```
PENDING → IN_PROGRESS → COMPLETED
         → CANCELLED
         → FAILED
         → INTERRUPTED → IN_PROGRESS (when resumed)
```

## 🔧 **Integration with Core Browser**

### **BrowserClient Integration**

The BrowserClient now integrates with MainWindow:

```cpp
// In BrowserClient::OnAfterCreated()
void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    if (main_window_) {
        main_window_->OnBrowserCreated(browser);
    }
}

// In BrowserClient::OnLoadingStateChange()
void BrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                        bool is_loading,
                                        bool can_go_back,
                                        bool can_go_forward) {
    if (main_window_) {
        main_window_->OnLoadingStateChange(is_loading, can_go_back, can_go_forward);
    }
}
```

### **DownloadHandler Integration**

The DownloadManager implements CefDownloadHandler:

```cpp
// In BrowserClient::GetDownloadHandler()
CefRefPtr<CefDownloadHandler> BrowserClient::GetDownloadHandler() {
    if (main_window_ && main_window_->GetDownloadManager()) {
        return main_window_->GetDownloadManager();
    }
    return nullptr;
}
```

### **Resource Request Handling**

Ad-blocker integration through RequestInterceptor:

```cpp
// In BrowserClient::GetResourceRequestHandler()
CefRefPtr<CefResourceRequestHandler> BrowserClient::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling) {
    
    auto ad_blocker = AdBlockerManager::GetInstance().GetAdBlocker();
    if (ad_blocker && ad_blocker->IsEnabled()) {
        auto interceptor = ad_blocker->GetRequestInterceptor();
        if (interceptor) {
            disable_default_handling = true;
            return interceptor->GetCefResourceRequestHandler();
        }
    }
    return nullptr;
}
```

## 🎨 **UI Architecture**

```
┌─────────────────────────────────────────────────────────────┐
│                        MainWindow                              │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                      Toolbar                               │ │
│  │  [←] [→] [↻] [🏠] [+] [🚫] [👨‍💻] [🔒] [🛡️] [⬇️] [⚙️]    │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                      TabStrip                               │ │
│  │  [🌐 Tab 1] [📄 Tab 2] [+]                                │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                    AddressBar                               │ │
│  │  [🔒 https://example.com]                                  │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                      Browser                               │ │
│  │  ┌─────────────────────────────────────────────────────┐ │ │
│  │  │                    Web Content                          │ │ │
│  │  │                                                         │ │ │
│  │  │                    Page Content                         │ │ │
│  │  │                                                         │ │ │
│  │  └─────────────────────────────────────────────────────┘ │ │
│  └─────────────────────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                    Status Bar (Future)                      │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## 📋 **Feature Summary**

### ✅ **Implemented Features**

1. **Polished UI**
   - MainWindow with coordinated components
   - Toolbar with all navigation and feature controls
   - TabStrip for multi-tab browsing
   - AddressBar with URL management
   - SettingsDialog for configuration
   - DownloadManager for file downloads

2. **Feature Toggles**
   - Ad-Blocker toggle (UI + command line)
   - Developer Tools toggle (UI + command line)
   - HTTPS-Only mode toggle (UI + command line)
   - Sandbox toggle (UI + command line)

3. **Tab Management**
   - Create new tabs
   - Close existing tabs
   - Switch between tabs
   - Tab title updates
   - Loading state indicators

4. **Download Management**
   - Start downloads
   - Cancel downloads
   - Pause/resume downloads
   - Progress tracking
   - Download history
   - Clear download lists

5. **Settings Management**
   - All settings configurable via UI
   - Settings organized in tabs
   - Apply/cancel functionality
   - Reset to defaults

### 🔄 **Integration Points**

- BrowserClient ↔ MainWindow
- DownloadManager ↔ CEF DownloadHandler
- AdBlocker ↔ RequestInterceptor
- Settings ↔ ConfigManager
- UI State ↔ Core Browser State

### 🎯 **Usage Examples**

**Creating a new tab:**
```cpp
main_window->CreateNewTab("https://example.com");
```

**Toggling ad-blocker:**
```cpp
main_window->ToggleAdBlocker();
```

**Starting a download:**
```cpp
main_window->GetDownloadManager()->StartDownload("https://example.com/file.zip");
```

**Opening settings:**
```cpp
main_window->OpenSettings();
```

**Opening downloads dialog:**
```cpp
main_window->OpenDownloads();
```

## 🚀 **Next Steps**

To make the UI fully functional, you'll need to implement the platform-specific UI code for your target platform (Windows, macOS, or Linux). The current implementation provides the architecture and logic, with placeholder platform-specific code.

### **Platform Implementation Required**

1. **Windows**: Implement using Win32 API or Windows UI framework
2. **macOS**: Implement using Cocoa or SwiftUI
3. **Linux**: Implement using GTK, Qt, or other Linux UI framework

### **Example: Windows Implementation**

For Windows, you would implement the platform-specific parts using Win32 API:

```cpp
// In toolbar.cpp for Windows
bool Toolbar::Initialize() {
    // Create toolbar window
    hwnd_toolbar_ = CreateWindowEx(
        0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT,
        0, 0, 0, 0, hwnd_parent_, NULL, GetModuleHandle(NULL), NULL
    );
    
    // Add buttons
    TBBUTTON buttons[] = {
        {0, 0, TBSTATE_ENABLED, BTNS_BUTTON, {0, 0}, 0, (INT_PTR)L"Back"},
        {1, 1, TBSTATE_ENABLED, BTNS_BUTTON, {0, 0}, 0, (INT_PTR)L"Forward"},
        // ... more buttons
    };
    
    SendMessage(hwnd_toolbar_, TB_ADDBUTTONS, (WPARAM)ARRAYSIZE(buttons), (LPARAM)buttons);
    
    return true;
}
```

## 📚 **API Reference**

### **MainWindow Class**

**Properties:**
- `BrowserClient* client_` - Associated browser client
- `std::vector<CefRefPtr<CefBrowser>> browsers_` - All browser instances
- `int current_tab_index_` - Currently active tab
- `bool adblocker_enabled_` - Ad-blocker state
- `bool devtools_enabled_` - DevTools state
- `bool https_only_enabled_` - HTTPS-only state
- `bool sandbox_enabled_` - Sandbox state

**Methods:**
- `bool Initialize(BrowserClient* client)` - Initialize with browser client
- `void Show()` - Show the window
- `void Hide()` - Hide the window
- `void Close()` - Close the window
- `void CreateNewTab(const std::string& url)` - Create new tab
- `void CloseTab(int index)` - Close tab at index
- `void SwitchToTab(int index)` - Switch to tab at index
- `size_t GetTabCount() const` - Get number of tabs
- `void LoadUrl(const std::string& url)` - Load URL in current tab
- `void ToggleAdBlocker()` - Toggle ad-blocker
- `void ToggleDevTools()` - Toggle developer tools
- `void ToggleHttpsOnly()` - Toggle HTTPS-only mode
- `void ToggleSandbox()` - Toggle sandbox
- `void OpenSettings()` - Open settings dialog
- `void OpenDownloads()` - Open downloads dialog
- `void UpdateUI()` - Update all UI components

### **Toolbar Class**

**Properties:**
- `MainWindow* main_window_` - Parent main window
- `bool can_go_back_` - Can navigate back
- `bool can_go_forward_` - Can navigate forward
- `bool is_loading_` - Page is loading
- `bool adblocker_enabled_` - Ad-blocker state
- `bool devtools_enabled_` - DevTools state
- `bool https_only_enabled_` - HTTPS-only state
- `bool sandbox_enabled_` - Sandbox state

**Methods:**
- `bool Initialize()` - Initialize toolbar
- `void Show()` - Show toolbar
- `void Hide()` - Hide toolbar
- `void UpdateNavigationButtons(bool can_go_back, bool can_go_forward)` - Update nav button states
- `void UpdateLoadingIndicator(bool is_loading)` - Update loading indicator
- `void UpdateAdBlockerToggle(bool enabled)` - Update ad-blocker toggle
- `void UpdateDevToolsToggle(bool enabled)` - Update DevTools toggle
- `void UpdateHttpsOnlyToggle(bool enabled)` - Update HTTPS-only toggle
- `void UpdateSandboxToggle(bool enabled)` - Update sandbox toggle

### **TabStrip Class**

**Properties:**
- `MainWindow* main_window_` - Parent main window
- `std::vector<TabInfo> tabs_` - All tabs
- `int active_tab_index_` - Currently active tab

**Methods:**
- `bool Initialize()` - Initialize tab strip
- `void Show()` - Show tab strip
- `void Hide()` - Hide tab strip
- `void AddTab(const std::string& title)` - Add new tab
- `void RemoveTab(int index)` - Remove tab at index
- `void SetActiveTab(int index)` - Set active tab
- `int GetActiveTab() const` - Get active tab index
- `size_t GetTabCount() const` - Get tab count
- `void SetTabTitle(int index, const std::string& title)` - Set tab title
- `void SetTabUrl(int index, const std::string& url)` - Set tab URL
- `void SetTabLoading(int index, bool is_loading)` - Set tab loading state

### **AddressBar Class**

**Properties:**
- `MainWindow* main_window_` - Parent main window
- `std::string current_url_` - Current URL
- `std::string entered_url_` - URL being entered
- `bool is_secure_` - Connection is secure
- `std::string domain_` - Current domain

**Methods:**
- `bool Initialize()` - Initialize address bar
- `void Show()` - Show address bar
- `void Hide()` - Hide address bar
- `void SetUrl(const std::string& url)` - Set URL
- `std::string GetUrl() const` - Get current URL
- `void SetFocus()` - Set focus to address bar
- `void OnUrlEntered(const std::string& url)` - Handle URL input
- `void OnUrlInputCancelled()` - Handle input cancellation
- `void UpdateSecurityIndicator(bool is_secure, const std::string& domain)` - Update security UI

### **SettingsDialog Class**

**Properties:**
- `MainWindow* main_window_` - Parent main window
- `SettingsTab active_tab_` - Currently active settings tab
- `SettingsState current_settings_` - Current settings being edited
- `SettingsState original_settings_` - Original settings for cancel

**Methods:**
- `bool Initialize()` - Initialize settings dialog
- `void Show()` - Show settings dialog
- `void Hide()` - Hide settings dialog
- `void SetActiveTab(SettingsTab tab)` - Set active tab
- `void ApplySettings()` - Apply and save settings
- `void Cancel()` - Cancel without saving
- `void ResetToDefaults()` - Reset to default settings
- `void LoadCurrentSettings()` - Load current settings
- `void SaveSettings()` - Save settings to config

### **DownloadManager Class**

**Properties:**
- `MainWindow* main_window_` - Parent main window
- `std::vector<DownloadInfo> downloads_` - All downloads
- `std::mutex mutex_` - Thread safety

**Methods:**
- `bool Initialize()` - Initialize download manager
- `void StartDownload(const std::string& url, const std::string& filename)` - Start download
- `void CancelDownload(const std::string& download_id)` - Cancel download
- `void PauseDownload(const std::string& download_id)` - Pause download
- `void ResumeDownload(const std::string& download_id)` - Resume download
- `std::vector<DownloadInfo> GetDownloadList() const` - Get all downloads
- `DownloadInfo GetDownload(const std::string& download_id) const` - Get specific download
- `void ClearCompletedDownloads()` - Clear completed downloads
- `void ClearAllDownloads()` - Clear all downloads
- `void ShowDownloadsDialog()` - Show downloads dialog
- `size_t GetDownloadCount() const` - Get download count
- `size_t GetActiveDownloadCount() const` - Get active download count

---

*This documentation provides a comprehensive overview of all UI features, toggles, tabs, and download functionality in NullWare Browser.*
