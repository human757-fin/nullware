#include <include/cef_app.h>
#include <include/cef_client.h>

#include "src/ui/main_window.h"
#include "src/browser/browser_client.h"
#include "src/common/logging.h"
#include "src/common/config.h"
#include "src/adblocker/ad_blocker.h"

#include <iostream>
#include <string>

namespace nullware {

// Application class
class NullWareApp : public CefApp,
                    public CefBrowserProcessHandler {
public:
    NullWareApp() = default;
    ~NullWareApp() override = default;
    
    // CefApp methods
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
        return this;
    }
    
    // CefBrowserProcessHandler methods
    virtual void OnContextInitialized() override {
        std::cout << "CEF context initialized" << std::endl;
        LOG_INFO("CEF context initialized");
        
        // Initialize our systems
        Logger::GetInstance().Initialize(
            LogLevel::INFO, 
            LogTarget::CONSOLE
        );
        
        // Load configuration from command line
        int argc = 0;
        char** argv = nullptr;
        ConfigManager::GetInstance().LoadFromCommandLine(argc, argv);
        
        // Initialize ad-blocker
        AdBlockerManager::GetInstance().Initialize();
        
        // Create browser client
        browser_client_ = new BrowserClient();
        
        // Create main window
        main_window_ = new MainWindow();
        if (main_window_->Initialize(browser_client_)) {
            // Set main window in browser client
            browser_client_->SetMainWindow(main_window_);
            
            // Show the main window
            main_window_->Show();
            
            std::cout << "NullWare Browser ready!" << std::endl;
            LOG_INFO("NullWare Browser ready");
        } else {
            std::cerr << "Failed to initialize main window" << std::endl;
            LOG_ERROR("Failed to initialize main window");
        }
    }
    
    virtual void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) override {
        
        // Parse command line arguments
        std::vector<std::string> args;
        for (int i = 0; i < command_line->GetArgc(); i++) {
            args.push_back(command_line->GetArgv()[i]);
        }
        
        // Convert to char* array for ConfigManager
        std::vector<char*> argv;
        for (auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        
        ConfigManager::GetInstance().LoadFromCommandLine(static_cast<int>(argv.size()), argv.data());
    }
    
private:
    MainWindow* main_window_;
    BrowserClient* browser_client_;
    
    IMPLEMENT_REFCOUNTING(NullWareApp);
};

}  // namespace nullware

// Entry point
int main(int argc, char* argv[]) {
    std::cout << "NullWare Browser - Starting..." << std::endl;
    std::cout << "Version: " NULLWARE_VERSION_STRING << std::endl;
    
    // CEF settings
    CefSettings settings;
    settings.no_sandbox = true; // Disable sandbox for simplicity in this example
    settings.multi_threaded_message_loop = true;
    settings.log_severity = LOGSEVERITY_INFO;
    settings.log_file = "nullware_debug.log";
    
    // Set user agent
    settings.user_agent = NULLWARE_USER_AGENT;
    
    // Initialize CEF
    CefMainArgs main_args(argc, argv);
    
    // Run CEF
    int exit_code = CefExecuteW(NullWareApp::GetInstance(), main_args);
    
    std::cout << "NullWare Browser - Exiting with code: " << exit_code << std::endl;
    
    return exit_code;
}
