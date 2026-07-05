#include "src/browser/browser_main.h"

#include "src/browser/browser_window.h"
#include "src/browser/browser_client.h"
#include "src/common/logging.h"
#include "src/common/config.h"

// CEF headers
#include <include/cef_app.h>
#include <include/cef_client.h>

namespace nullware {

BrowserMain::BrowserMain() : initialized_(false) {}

BrowserMain::~BrowserMain() {
    Shutdown();
}

bool BrowserMain::Initialize(int argc, char* argv[]) {
    LOG_INFO("Initializing BrowserMain");
    
    if (initialized_) {
        LOG_WARNING("BrowserMain already initialized");
        return true;
    }
    
    // Parse command line
    ParseCommandLine(argc, argv);
    
    // Initialize CEF
    if (!InitializeCef()) {
        LOG_ERROR("Failed to initialize CEF");
        return false;
    }
    
    // Create browser client
    browser_client_ = std::make_unique<BrowserClient>();
    
    // Create first window
    CreateFirstWindow();
    
    initialized_ = true;
    LOG_INFO("BrowserMain initialized successfully");
    
    return true;
}

int BrowserMain::Run() {
    if (!initialized_) {
        LOG_ERROR("BrowserMain not initialized");
        return 1;
    }
    
    LOG_INFO("Running browser main loop");
    
    // Run CEF message loop
    CefRunMessageLoop();
    
    LOG_INFO("Browser main loop exited");
    
    return 0;
}

void BrowserMain::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    LOG_INFO("Shutting down BrowserMain");
    
    // Close all windows
    CloseAllWindows();
    
    // Shutdown CEF
    ShutdownCef();
    
    initialized_ = false;
    LOG_INFO("BrowserMain shutdown complete");
}

BrowserClient* BrowserMain::GetBrowserClient() {
    return browser_client_.get();
}

BrowserWindow* BrowserMain::CreateBrowserWindow(const std::string& url) {
    LOG_INFO("Creating new browser window");
    
    auto window = std::make_unique<BrowserWindow>(browser_client_.get());
    BrowserWindow* window_ptr = window.get();
    
    if (!url.empty()) {
        window->LoadUrl(url);
    } else {
        // Load default homepage
        const auto& config = ConfigManager::GetInstance().GetConfig();
        window->LoadUrl(config.default_homepage);
    }
    
    windows_.push_back(std::move(window));
    
    LOG_INFO("Browser window created, total windows: " + std::to_string(windows_.size()));
    
    return window_ptr;
}

void BrowserMain::CloseAllWindows() {
    LOG_INFO("Closing all browser windows");
    
    for (auto& window : windows_) {
        window->Close();
    }
    
    windows_.clear();
    
    LOG_INFO("All browser windows closed");
}

size_t BrowserMain::GetWindowCount() const {
    return windows_.size();
}

void BrowserMain::ParseCommandLine(int argc, char* argv[]) {
    LOG_INFO("Parsing command line arguments");
    
    // Command line parsing is already handled by ConfigManager
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    // Log relevant configuration
    LOG_INFO("AdBlocker enabled: " + std::string(config.enable_adblocker ? "true" : "false"));
    LOG_INFO("DevTools enabled: " + std::string(config.enable_devtools ? "true" : "false"));
    LOG_INFO("Sandbox enabled: " + std::string(config.sandbox_enabled ? "true" : "false"));
    LOG_INFO("HTTPS only mode: " + std::string(config.https_only_mode ? "true" : "false"));
}

bool BrowserMain::InitializeCef() {
    LOG_INFO("Initializing CEF");
    
    // CEF settings
    CefSettings settings;
    
    // Set basic settings
    settings.no_sandbox = !ConfigManager::GetInstance().GetConfig().sandbox_enabled;
    settings.multi_threaded_message_loop = true;
    settings.log_severity = LOGSEVERITY_INFO;
    settings.log_file = "nullware_debug.log";
    
    // Set user agent
    settings.user_agent = NULLWARE_USER_AGENT;
    
    // Set cache path
    const auto& config = ConfigManager::GetInstance().GetConfig();
    settings.cache_path = config.cache_dir;
    
    // Set locale
    settings.locale = "en-US";
    
    // Initialize CEF
    CefInitialize(settings, nullptr, nullptr);
    
    LOG_INFO("CEF initialized successfully");
    
    return true;
}

void BrowserMain::ShutdownCef() {
    LOG_INFO("Shutting down CEF");
    
    CefShutdown();
    
    LOG_INFO("CEF shutdown complete");
}

void BrowserMain::CreateFirstWindow() {
    LOG_INFO("Creating first browser window");
    
    const auto& config = ConfigManager::GetInstance().GetConfig();
    
    // Create the first window
    auto window = std::make_unique<BrowserWindow>(browser_client_.get());
    
    // Load default URL
    if (!config.startup_pages.empty()) {
        window->LoadUrl(config.startup_pages);
    } else {
        window->LoadUrl(config.default_homepage);
    }
    
    windows_.push_back(std::move(window));
    
    LOG_INFO("First browser window created");
}

}  // namespace nullware
