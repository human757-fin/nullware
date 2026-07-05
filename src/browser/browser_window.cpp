#include "src/browser/browser_window.h"

#include "src/browser/browser_client.h"
#include "src/common/logging.h"
#include "src/common/config.h"

// Platform-specific includes
#ifdef NULLWARE_PLATFORM_WINDOWS
#include <windows.h>
#elif defined(NULLWARE_PLATFORM_LINUX)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#elif defined(NULLWARE_PLATFORM_MACOS)
#include <Cocoa/Cocoa.h>
#endif

namespace nullware {

BrowserWindow::BrowserWindow(BrowserClient* client)
    : client_(client),
      width_(1024),
      height_(768),
      x_(0),
      y_(0),
      visible_(false),
      maximized_(false),
      minimized_(false),
      fullscreen_(false) {
    
    // Set default title
    title_ = "NullWare Browser";
}

BrowserWindow::~BrowserWindow() {
    // Clean up
    if (browser_) {
        browser_->GetHost()->CloseBrowser(true);
        browser_ = nullptr;
    }
}

bool BrowserWindow::Initialize() {
    LOG_INFO("Initializing browser window");
    
    // Create browser settings
    CefBrowserSettings browser_settings;
    
    // Configure based on settings
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    // Enable developer tools if configured
    browser_settings.webgl = config.experimental_webgl ? STATE_ENABLED : STATE_DISABLED;
    browser_settings.webrtc = config.experimental_webrtc ? STATE_ENABLED : STATE_DISABLED;
    
    // Create the browser
    CefWindowInfo window_info;
    
    // Set up window info based on platform
    #ifdef NULLWARE_PLATFORM_WINDOWS
    // Windows implementation
    window_info.SetAsWindowless(nullptr);
    #elif defined(NULLWARE_PLATFORM_LINUX)
    // Linux implementation
    window_info.SetAsWindowless(nullptr);
    #elif defined(NULLWARE_PLATFORM_MACOS)
    // macOS implementation
    window_info.SetAsWindowless(nullptr);
    #endif
    
    // Create the browser
    CefBrowserHost::CreateBrowser(window_info, client_, 
                                  CefString("about:blank"), browser_settings, nullptr);
    
    LOG_INFO("Browser window initialized");
    return true;
}

void BrowserWindow::Show() {
    if (visible_) return;
    
    LOG_INFO("Showing browser window");
    
    if (browser_) {
        // Show the browser
        browser_->GetHost()->WasResized();
        browser_->GetHost()->NotifyScreenInfoChanged();
    }
    
    visible_ = true;
}

void BrowserWindow::Hide() {
    if (!visible_) return;
    
    LOG_INFO("Hiding browser window");
    visible_ = false;
}

void BrowserWindow::Close() {
    LOG_INFO("Closing browser window");
    
    if (browser_) {
        browser_->GetHost()->CloseBrowser(false);
    }
}

void BrowserWindow::LoadUrl(const std::string& url) {
    LOG_INFO("Loading URL: " + url);
    
    if (browser_) {
        CefRefPtr<CefFrame> frame = browser_->GetMainFrame();
        if (frame) {
            frame->LoadURL(CefString(url));
        }
    }
}

BrowserClient* BrowserWindow::GetClient() {
    return client_;
}

CefRefPtr<CefBrowser> BrowserWindow::GetBrowser() {
    return browser_;
}

void BrowserWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser created in window");
    browser_ = browser;
    
    // Set the browser in the client
    if (client_) {
        client_->SetMainBrowser(browser);
    }
    
    // Load the default URL
    const auto& config = ConfigManager::GetInstance().GetConfig();
    if (!config.default_homepage.empty()) {
        LoadUrl(config.default_homepage);
    }
}

void BrowserWindow::OnBrowserCloseRequested(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser close requested in window");
    // Allow the browser to close
}

void BrowserWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
    LOG_INFO("Browser closed in window");
    
    if (browser == browser_) {
        browser_ = nullptr;
    }
}

void BrowserWindow::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool is_loading,
                                         bool can_go_back,
                                         bool can_go_forward) {
    // Update UI state based on loading state
    LOG_DEBUG("Loading state changed: loading=" + std::string(is_loading ? "true" : "false") +
               ", can_go_back=" + std::string(can_go_back ? "true" : "false") +
               ", can_go_forward=" + std::string(can_go_forward ? "true" : "false"));
}

void BrowserWindow::OnLoadStart(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               TransitionType transition_type) {
    LOG_DEBUG("Load started: " + frame->GetURL().ToString());
}

void BrowserWindow::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             int http_status_code) {
    LOG_DEBUG("Load ended: " + frame->GetURL().ToString() + 
               " with status: " + std::to_string(http_status_code));
}

void BrowserWindow::OnLoadError(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               ErrorCode error_code,
                               const CefString& error_text,
                               const CefString& failed_url) {
    LOG_ERROR("Load error: " + error_text.ToString() + 
               " (" + std::to_string(error_code) + ")");
}

void BrowserWindow::SetTitle(const std::string& title) {
    title_ = title;
    LOG_DEBUG("Window title set to: " + title);
}

void BrowserWindow::SetSize(int width, int height) {
    width_ = width;
    height_ = height;
    LOG_DEBUG("Window size set to: " + std::to_string(width) + "x" + std::to_string(height));
    
    if (browser_) {
        browser_->GetHost()->WasResized();
    }
}

void BrowserWindow::SetPosition(int x, int y) {
    x_ = x;
    y_ = y;
    LOG_DEBUG("Window position set to: (" + std::to_string(x) + "," + std::to_string(y) + ")");
}

void BrowserWindow::Maximize() {
    LOG_DEBUG("Window maximized");
    maximized_ = true;
}

void BrowserWindow::Minimize() {
    LOG_DEBUG("Window minimized");
    minimized_ = true;
}

void BrowserWindow::Restore() {
    LOG_DEBUG("Window restored");
    maximized_ = false;
    minimized_ = false;
}

void BrowserWindow::GoBack() {
    LOG_DEBUG("Navigating back");
    if (browser_) {
        browser_->GoBack();
    }
}

void BrowserWindow::GoForward() {
    LOG_DEBUG("Navigating forward");
    if (browser_) {
        browser_->GoForward();
    }
}

void BrowserWindow::Reload() {
    LOG_DEBUG("Reloading page");
    if (browser_) {
        CefRefPtr<CefFrame> frame = browser_->GetMainFrame();
        if (frame) {
            frame->Reload();
        }
    }
}

void BrowserWindow::StopLoad() {
    LOG_DEBUG("Stopping page load");
    if (browser_) {
        browser_->StopLoad();
    }
}

bool BrowserWindow::IsVisible() const {
    return visible_;
}

bool BrowserWindow::IsMaximized() const {
    return maximized_;
}

bool BrowserWindow::IsMinimized() const {
    return minimized_;
}

bool BrowserWindow::IsFullscreen() const {
    return fullscreen_;
}

}  // namespace nullware
