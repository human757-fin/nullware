#ifndef NULLWARE_BROWSER_BROWSER_MAIN_H_
#define NULLWARE_BROWSER_BROWSER_MAIN_H_

#include <string>
#include <vector>
#include <memory>

#include "src/common/config.h"

namespace nullware {

class BrowserWindow;
class BrowserClient;

// Main browser application class
class BrowserMain {
public:
    BrowserMain();
    ~BrowserMain();
    
    // Initialize the browser
    bool Initialize(int argc, char* argv[]);
    
    // Run the browser main loop
    int Run();
    
    // Shutdown the browser
    void Shutdown();
    
    // Get the browser client
    BrowserClient* GetBrowserClient();
    
    // Create a new browser window
    BrowserWindow* CreateBrowserWindow(const std::string& url = "");
    
    // Close all browser windows
    void CloseAllWindows();
    
    // Get the number of open windows
    size_t GetWindowCount() const;
    
private:
    std::vector<std::unique_ptr<BrowserWindow>> windows_;
    std::unique_ptr<BrowserClient> browser_client_;
    bool initialized_;
    
    // Parse command line arguments
    void ParseCommandLine(int argc, char* argv[]);
    
    // Initialize CEF (Chromium Embedded Framework)
    bool InitializeCef();
    
    // Shutdown CEF
    void ShutdownCef();
    
    // Create the first browser window
    void CreateFirstWindow();
};

}  // namespace nullware

#endif  // NULLWARE_BROWSER_BROWSER_MAIN_H_
