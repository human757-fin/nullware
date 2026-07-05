#include "src/ui/address_bar.h"

#include "src/ui/main_window.h"
#include "src/common/logging.h"
#include "src/common/string_utils.h"

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

AddressBar::AddressBar(MainWindow* main_window)
    : main_window_(main_window),
      is_secure_(false),
      native_handle_(nullptr) {}

AddressBar::~AddressBar() {
    // Clean up platform-specific resources
}

bool AddressBar::Initialize() {
    LOG_INFO("Initializing AddressBar");
    
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

void AddressBar::Show() {
    LOG_DEBUG("Showing AddressBar");
    // Platform-specific show implementation
}

void AddressBar::Hide() {
    LOG_DEBUG("Hiding AddressBar");
    // Platform-specific hide implementation
}

void AddressBar::SetUrl(const std::string& url) {
    LOG_DEBUG("Setting URL: " + url);
    current_url_ = url;
    entered_url_ = url;
    
    // Update security indicator
    bool is_secure = StartsWith(ToLower(url), "https://");
    std::string domain = ExtractDomain(url);
    UpdateSecurityIndicator(is_secure, domain);
    
    // Platform-specific URL update
}

std::string AddressBar::GetUrl() const {
    return current_url_;
}

void AddressBar::SetFocus() {
    LOG_DEBUG("Setting focus to AddressBar");
    // Platform-specific focus implementation
}

void AddressBar::OnUrlEntered(const std::string& url) {
    LOG_DEBUG("URL entered: " + url);
    
    if (main_window_) {
        // Add http:// prefix if no scheme is specified
        std::string processed_url = url;
        if (!Contains(url, "://") && !StartsWith(url, "about:") && 
            !StartsWith(url, "data:") && !StartsWith(url, "javascript:")) {
            processed_url = "https://" + url;
        }
        
        main_window_->LoadUrl(processed_url);
    }
}

void AddressBar::OnUrlInputCancelled() {
    LOG_DEBUG("URL input cancelled");
    // Reset to current URL
    entered_url_ = current_url_;
}

void AddressBar::UpdateSecurityIndicator(bool is_secure, const std::string& domain) {
    is_secure_ = is_secure;
    domain_ = domain;
    
    LOG_DEBUG("Security indicator updated: secure=" + std::string(is_secure ? "true" : "false") +
               ", domain=" + domain);
    
    // Platform-specific security indicator update
}

}  // namespace nullware
