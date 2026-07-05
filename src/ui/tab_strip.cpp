#include "src/ui/tab_strip.h"

#include "src/ui/main_window.h"
#include "src/common/logging.h"

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

TabStrip::TabStrip(MainWindow* main_window)
    : main_window_(main_window),
      active_tab_index_(-1),
      native_handle_(nullptr) {}

TabStrip::~TabStrip() {
    // Clean up platform-specific resources
}

bool TabStrip::Initialize() {
    LOG_INFO("Initializing TabStrip");
    
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

void TabStrip::Show() {
    LOG_DEBUG("Showing TabStrip");
    // Platform-specific show implementation
}

void TabStrip::Hide() {
    LOG_DEBUG("Hiding TabStrip");
    // Platform-specific hide implementation
}

void TabStrip::AddTab(const std::string& title) {
    LOG_DEBUG("Adding tab: " + title);
    
    TabInfo tab;
    tab.title = title;
    tab.url = "";
    tab.is_active = tabs_.empty(); // Active if first tab
    tab.is_loading = false;
    tab.favicon_id = -1;
    
    tabs_.push_back(tab);
    
    if (tabs_.size() == 1) {
        active_tab_index_ = 0;
    }
    
    // Platform-specific tab creation
}

void TabStrip::RemoveTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) {
        return;
    }
    
    LOG_DEBUG("Removing tab at index: " + std::to_string(index));
    
    tabs_.erase(tabs_.begin() + index);
    
    // Adjust active tab index
    if (active_tab_index_ >= index) {
        active_tab_index_ = std::max(-1, active_tab_index_ - 1);
    }
    
    // If no tabs left, reset
    if (tabs_.empty()) {
        active_tab_index_ = -1;
    }
    
    // Platform-specific tab removal
}

void TabStrip::SetActiveTab(int index) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) {
        return;
    }
    
    LOG_DEBUG("Setting active tab to index: " + std::to_string(index));
    
    // Deactivate current active tab
    if (active_tab_index_ >= 0 && active_tab_index_ < static_cast<int>(tabs_.size())) {
        tabs_[active_tab_index_].is_active = false;
    }
    
    // Activate new tab
    active_tab_index_ = index;
    tabs_[index].is_active = true;
    
    // Notify main window
    if (main_window_) {
        main_window_->SwitchToTab(index);
    }
    
    // Platform-specific activation
}

int TabStrip::GetActiveTab() const {
    return active_tab_index_;
}

size_t TabStrip::GetTabCount() const {
    return tabs_.size();
}

void TabStrip::SetTabTitle(int index, const std::string& title) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) {
        return;
    }
    
    LOG_DEBUG("Setting tab title at index " + std::to_string(index) + " to: " + title);
    tabs_[index].title = title;
    
    // Platform-specific title update
}

void TabStrip::SetTabUrl(int index, const std::string& url) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) {
        return;
    }
    
    LOG_DEBUG("Setting tab URL at index " + std::to_string(index) + " to: " + url);
    tabs_[index].url = url;
}

void TabStrip::SetTabLoading(int index, bool is_loading) {
    if (index < 0 || index >= static_cast<int>(tabs_.size())) {
        return;
    }
    
    LOG_DEBUG("Setting tab loading state at index " + std::to_string(index) + 
               " to: " + std::string(is_loading ? "true" : "false"));
    tabs_[index].is_loading = is_loading;
    
    // Platform-specific loading state update
}

void TabStrip::OnTabClicked(int index) {
    LOG_DEBUG("Tab clicked at index: " + std::to_string(index));
    SetActiveTab(index);
}

void TabStrip::OnTabCloseClicked(int index) {
    LOG_DEBUG("Tab close clicked at index: " + std::to_string(index));
    
    if (main_window_) {
        main_window_->CloseTab(index);
    }
}

void TabStrip::OnNewTabClicked() {
    LOG_DEBUG("New tab button clicked");
    
    if (main_window_) {
        main_window_->CreateNewTab();
    }
}

}  // namespace nullware
