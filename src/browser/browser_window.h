#ifndef NULLWARE_BROWSER_BROWSER_WINDOW_H_
#define NULLWARE_BROWSER_BROWSER_WINDOW_H_

#include <include/cef_base.h>
#include <include/cef_browser.h>

#include <string>
#include <memory>

namespace nullware {

class BrowserClient;

// Browser window interface
class BrowserWindow {
public:
    BrowserWindow(BrowserClient* client);
    virtual ~BrowserWindow();
    
    // Initialize the window
    virtual bool Initialize();
    
    // Show the window
    virtual void Show();
    
    // Hide the window
    virtual void Hide();
    
    // Close the window
    virtual void Close();
    
    // Load a URL
    virtual void LoadUrl(const std::string& url);
    
    // Get the browser client
    BrowserClient* GetClient();
    
    // Get the CEF browser
    CefRefPtr<CefBrowser> GetBrowser();
    
    // Browser event handlers
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
    virtual void OnBrowserCloseRequested(CefRefPtr<CefBrowser> browser);
    virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser);
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                      bool is_loading,
                                      bool can_go_back,
                                      bool can_go_forward);
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type);
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int http_status_code);
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode error_code,
                           const CefString& error_text,
                           const CefString& failed_url);
    
    // Window management
    virtual void SetTitle(const std::string& title);
    virtual void SetSize(int width, int height);
    virtual void SetPosition(int x, int y);
    virtual void Maximize();
    virtual void Minimize();
    virtual void Restore();
    
    // Navigation
    virtual void GoBack();
    virtual void GoForward();
    virtual void Reload();
    virtual void StopLoad();
    
    // Get window state
    virtual bool IsVisible() const;
    virtual bool IsMaximized() const;
    virtual bool IsMinimized() const;
    virtual bool IsFullscreen() const;
    
protected:
    BrowserClient* client_;
    CefRefPtr<CefBrowser> browser_;
    std::string title_;
    int width_;
    int height_;
    int x_;
    int y_;
    bool visible_;
    bool maximized_;
    bool minimized_;
    bool fullscreen_;
};

}  // namespace nullware

#endif  // NULLWARE_BROWSER_BROWSER_WINDOW_H_
