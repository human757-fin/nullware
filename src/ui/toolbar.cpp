#include "src/ui/toolbar.h"

#include "src/ui/main_window.h"
#include "src/common/logging.h"

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

Toolbar::Toolbar(MainWindow* main_window)
    : main_window_(main_window),
      can_go_back_(false),
      can_go_forward_(false),
      is_loading_(false),
      adblocker_enabled_(true),
      devtools_enabled_(true),
      https_only_enabled_(false),
      sandbox_enabled_(true),
      native_handle_(nullptr) {}

Toolbar::~Toolbar() {
    // Clean up platform-specific resources
}

bool Toolbar::Initialize() {
    LOG_INFO("Initializing Toolbar");
    
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

void Toolbar::Show() {
    LOG_DEBUG("Showing Toolbar");
    // Platform-specific show implementation
}

void Toolbar::Hide() {
    LOG_DEBUG("Hiding Toolbar");
    // Platform-specific hide implementation
}

void Toolbar::UpdateNavigationButtons(bool can_go_back, bool can_go_forward) {
    can_go_back_ = can_go_back;
    can_go_forward_ = can_go_forward;
    LOG_DEBUG("Navigation buttons updated: back=" + std::string(can_go_back ? "true" : "false") +
               ", forward=" + std::string(can_go_forward ? "true" : "false"));
}

void Toolbar::UpdateLoadingIndicator(bool is_loading) {
    is_loading_ = is_loading;
    LOG_DEBUG("Loading indicator updated: " + std::string(is_loading ? "true" : "false"));
}

void Toolbar::UpdateAdBlockerToggle(bool enabled) {
    adblocker_enabled_ = enabled;
    LOG_DEBUG("Ad-blocker toggle updated: " + std::string(enabled ? "true" : "false"));
}

void Toolbar::UpdateDevToolsToggle(bool enabled) {
    devtools_enabled_ = enabled;
    LOG_DEBUG("DevTools toggle updated: " + std::string(enabled ? "true" : "false"));
}

void Toolbar::UpdateHttpsOnlyToggle(bool enabled) {
    https_only_enabled_ = enabled;
    LOG_DEBUG("HTTPS-only toggle updated: " + std::string(enabled ? "true" : "false"));
}

void Toolbar::UpdateSandboxToggle(bool enabled) {
    sandbox_enabled_ = enabled;
    LOG_DEBUG("Sandbox toggle updated: " + std::string(enabled ? "true" : "false"));
}

void Toolbar::OnBackClicked() {
    LOG_DEBUG("Back button clicked");
    if (main_window_) {
        if (main_window_->GetTabCount() > 0) {
            CefRefPtr<CefBrowser> browser = main_window_->GetClient()->GetMainBrowser();
            if (browser) {
                browser->GoBack();
            }
        }
    }
}

void Toolbar::OnForwardClicked() {
    LOG_DEBUG("Forward button clicked");
    if (main_window_) {
        if (main_window_->GetTabCount() > 0) {
            CefRefPtr<CefBrowser> browser = main_window_->GetClient()->GetMainBrowser();
            if (browser) {
                browser->GoForward();
            }
        }
    }
}

void Toolbar::OnRefreshClicked() {
    LOG_DEBUG("Refresh button clicked");
    if (main_window_) {
        if (main_window_->GetTabCount() > 0) {
            CefRefPtr<CefBrowser> browser = main_window_->GetClient()->GetMainBrowser();
            if (browser) {
                CefRefPtr<CefFrame> frame = browser->GetMainFrame();
                if (frame) {
                    frame->Reload();
                }
            }
        }
    }
}

void Toolbar::OnHomeClicked() {
    LOG_DEBUG("Home button clicked");
    if (main_window_) {
        const auto& config = ConfigManager::GetInstance().GetConfig();
        main_window_->LoadUrl(config.default_homepage);
    }
}

void Toolbar::OnNewTabClicked() {
    LOG_DEBUG("New tab button clicked");
    if (main_window_) {
        main_window_->CreateNewTab();
    }
}

void Toolbar::OnAdBlockerClicked() {
    LOG_DEBUG("Ad-blocker button clicked");
    if (main_window_) {
        main_window_->ToggleAdBlocker();
    }
}

void Toolbar::OnDevToolsClicked() {
    LOG_DEBUG("DevTools button clicked");
    if (main_window_) {
        main_window_->ToggleDevTools();
    }
}

void Toolbar::OnHttpsOnlyClicked() {
    LOG_DEBUG("HTTPS-only button clicked");
    if (main_window_) {
        main_window_->ToggleHttpsOnly();
    }
}

void Toolbar::OnSandboxClicked() {
    LOG_DEBUG("Sandbox button clicked");
    if (main_window_) {
        main_window_->ToggleSandbox();
    }
}

void Toolbar::OnDownloadsClicked() {
    LOG_DEBUG("Downloads button clicked");
    if (main_window_) {
        main_window_->OpenDownloads();
    }
}

void Toolbar::OnSettingsClicked() {
    LOG_DEBUG("Settings button clicked");
    if (main_window_) {
        main_window_->OpenSettings();
    }
}

}  // namespace nullware
