#ifndef NULLWARE_UI_TAB_STRIP_H_
#define NULLWARE_UI_TAB_STRIP_H_

#include <string>
#include <vector>
#include <functional>

namespace nullware {

class MainWindow;

// Tab information
struct TabInfo {
    std::string title;
    std::string url;
    bool is_active;
    bool is_loading;
    int favicon_id;
};

// Tab strip class
class TabStrip {
public:
    TabStrip(MainWindow* main_window);
    ~TabStrip();
    
    // Initialize the tab strip
    bool Initialize();
    
    // Show the tab strip
    void Show();
    
    // Hide the tab strip
    void Hide();
    
    // Add a new tab
    void AddTab(const std::string& title);
    
    // Remove a tab
    void RemoveTab(int index);
    
    // Set active tab
    void SetActiveTab(int index);
    
    // Get active tab index
    int GetActiveTab() const;
    
    // Get tab count
    size_t GetTabCount() const;
    
    // Set tab title
    void SetTabTitle(int index, const std::string& title);
    
    // Set tab URL
    void SetTabUrl(int index, const std::string& url);
    
    // Set tab loading state
    void SetTabLoading(int index, bool is_loading);
    
    // Tab click handler
    void OnTabClicked(int index);
    
    // Tab close handler
    void OnTabCloseClicked(int index);
    
    // New tab handler
    void OnNewTabClicked();
    
private:
    MainWindow* main_window_;
    std::vector<TabInfo> tabs_;
    int active_tab_index_;
    
    // Platform-specific implementation
    void* native_handle_;
};

}  // namespace nullware

#endif  // NULLWARE_UI_TAB_STRIP_H_
