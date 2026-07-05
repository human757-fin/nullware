#ifndef NULLWARE_UI_ADDRESS_BAR_H_
#define NULLWARE_UI_ADDRESS_BAR_H_

#include <string>
#include <functional>

namespace nullware {

class MainWindow;

// Address bar class
class AddressBar {
public:
    AddressBar(MainWindow* main_window);
    ~AddressBar();
    
    // Initialize the address bar
    bool Initialize();
    
    // Show the address bar
    void Show();
    
    // Hide the address bar
    void Hide();
    
    // Set the URL
    void SetUrl(const std::string& url);
    
    // Get the current URL
    std::string GetUrl() const;
    
    // Set focus to the address bar
    void SetFocus();
    
    // Handle URL input
    void OnUrlEntered(const std::string& url);
    
    // Handle URL input cancellation
    void OnUrlInputCancelled();
    
    // Update security indicators
    void UpdateSecurityIndicator(bool is_secure, const std::string& domain);
    
private:
    MainWindow* main_window_;
    std::string current_url_;
    std::string entered_url_;
    bool is_secure_;
    std::string domain_;
    
    // Platform-specific implementation
    void* native_handle_;
};

}  // namespace nullware

#endif  // NULLWARE_UI_ADDRESS_BAR_H_
