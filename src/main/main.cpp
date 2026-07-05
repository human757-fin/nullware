#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>

#include "src/common/logging.h"
#include "src/common/config.h"
#include "src/adblocker/ad_blocker.h"
#include "src/browser/browser_client.h"

#include <iostream>
#include <string>

// Simple application that creates a browser window
class SimpleHandler : public CefClient,
                      public CefLifeSpanHandler,
                      public CefLoadHandler {
public:
    SimpleHandler() = default;
    ~SimpleHandler() override = default;
    
    // CefClient methods
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }
    
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
        return this;
    }
    
    // CefLifeSpanHandler methods
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
        std::cout << "Browser created successfully!" << std::endl;
    }
    
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override {
        return false; // Allow browser to close
    }
    
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
        std::cout << "Browser closing..." << std::endl;
    }
    
    // CefLoadHandler methods
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) override {
        std::cout << "Loading: " << frame->GetURL().ToString() << std::endl;
    }
    
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int http_status_code) override {
        std::cout << "Loaded: " << frame->GetURL().ToString() 
                  << " (Status: " << http_status_code << ")" << std::endl;
    }
    
    IMPLEMENT_REFCOUNTING(SimpleHandler);
};

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
        
        // Initialize our systems
        nullware::Logger::GetInstance().Initialize(
            nullware::LogLevel::INFO, 
            nullware::LogTarget::CONSOLE
        );
        
        nullware::ConfigManager::GetInstance().LoadFromCommandLine(0, nullptr);
        
        // Initialize ad-blocker
        nullware::AdBlockerManager::GetInstance().Initialize();
        
        std::cout << "NullWare systems initialized" << std::endl;
        
        // Create browser window
        CefWindowInfo window_info;
        window_info.SetAsWindowless(nullptr); // Windowless rendering for simplicity
        
        CefBrowserSettings browser_settings;
        
        // Create the browser
        CefBrowserHost::CreateBrowser(window_info, 
                                      new SimpleHandler(), 
                                      CefString("https://www.google.com"), 
                                      browser_settings, 
                                      nullptr);
    }
    
    IMPLEMENT_REFCOUNTING(NullWareApp);
};

// Entry point
int main(int argc, char* argv[]) {
    std::cout << "NullWare Browser - Starting..." << std::endl;
    std::cout << "Version: " NULLWARE_VERSION_STRING << std::endl;
    
    // CEF settings
    CefSettings settings;
    settings.no_sandbox = true; // Disable sandbox for simplicity in this example
    settings.multi_threaded_message_loop = true;
    settings.log_severity = LOGSEVERITY_INFO;
    
    // Initialize CEF
    CefMainArgs main_args(argc, argv);
    int exit_code = CefExecuteW(NullWareApp::GetInstance(), main_args);
    
    std::cout << "NullWare Browser - Exiting with code: " << exit_code << std::endl;
    
    return exit_code;
}
